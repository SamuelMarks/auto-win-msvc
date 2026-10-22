#!/usr/bin/env python3
"""
Automated Source Transformation Engine for MSVC (auto-win-msvc).
Transforms unsupported C99/GNU constructs (such as Variable Length Arrays,
Elvis operators, self-initialization idioms, GCC statement expressions,
PACKED structs, and macro/enum collisions) into strict C89 / MSVC-compatible
constructs at build time.
"""

import argparse
import os
import re
import sys

NL = chr(10)
TAB = chr(9)
BS = chr(92)


def _transform_packed_structs(content: str) -> str:
    """
    Transform GCC PACKED struct/union declarations into MSVC #pragma pack(push, 1).

    Args:
        content: C source or header content.

    Returns:
        Content with PACKED structs wrapped in #ifdef _MSC_VER #pragma pack(push, 1).
    """
    lines = content.splitlines(keepends=True)
    result = []
    i = 0
    n = len(lines)
    while i < n:
        line = lines[i]
        stripped = line.strip()
        if re.match(
            r"^(?:typedef\s+)?(?:struct|union)(?:\s+[a-zA-Z0-9_]+)?\s*(?:\{|$)",
            stripped,
        ):
            depth = line.count("{") - line.count("}")
            seen_brace = "{" in line
            block = [line]
            j = i + 1
            while j < n and (not seen_brace or depth > 0):
                if ";" in lines[j] and not seen_brace:
                    break
                if "{" in lines[j]:
                    seen_brace = True
                depth += lines[j].count("{") - lines[j].count("}")
                block.append(lines[j])
                j += 1
            block_text = "".join(block)
            if seen_brace and "PACKED" in block_text:
                result.append(
                    NL.join(
                        [
                            "#ifdef _MSC_VER",
                            "#pragma pack(push, 1)",
                            "#endif",
                            "",
                        ]
                    )
                )
                result.extend(block)
                result.append(
                    NL.join(
                        [
                            "#ifdef _MSC_VER",
                            "#pragma pack(pop)",
                            "#endif",
                            "",
                        ]
                    )
                )
                i = j
                continue
        result.append(line)
        i += 1
    return "".join(result)


def transform_source_content(content: str) -> str:
    """
    Apply AST-like heuristic rewrites to eliminate VLAs and GNU C idioms.

    Args:
        content: Raw C source or header file string.

    Returns:
        Transformed C source or header string compatible with MSVC and C89.
    """
    has_alloca = False

    def vla_alloca_repl(match: re.Match[str]) -> str:
        nonlocal has_alloca
        indent = match.group(1)
        type_str = match.group(2)
        var_name = match.group(3)
        expr = match.group(4).strip()
        expr_no_comments = re.sub(r"/\*.*?\*/", "", expr).strip()
        if (
            re.match(
                r"^(?:0x[0-9a-fA-F]+|[0-9A-Z_+\-*/ ()\t?:=<>!&|~])+$",
                expr_no_comments,
            )
            or "sizeof" in expr_no_comments
            or "ARRAY_SIZE" in expr_no_comments
        ):
            return match.group(0)
        has_alloca = True
        return (
            f"{indent}{type_str} *{var_name} = "
            f"({type_str} *)alloca(({expr}) * sizeof({type_str}));"
        )

    generic_vla_pattern = re.compile(
        r"^(\s+)((?:unsigned\s+)?(?:char|int|uint8_t|uint16_t|uint32_t|int32_t|const\s+char\s*\*|struct\s+[a-zA-Z0-9_]+\s*\*)(?:\s*\*+)?)\s*([a-zA-Z0-9_]+)\[([^\x0a\]]+)\];",
        re.MULTILINE,
    )
    content = generic_vla_pattern.sub(vla_alloca_repl, content)

    if has_alloca and "#include <malloc.h>" not in content:
        content = (
            NL.join(
                [
                    "/* clang-format off */",
                    "#if defined(_MSC_VER) || defined(_WIN32)",
                    "#include <malloc.h>",
                    "#else",
                    "#include <alloca.h>",
                    "#endif",
                    "/* clang-format on */",
                    "",
                ]
            )
            + content
        )

    elvis_pattern = re.compile(
        r'("(?:\\.|[^"\\\x0a])*"|/\*[\s\S]*?\*/|//[^\x0a]*)|(\([a-zA-Z0-9_ %+\-*/&|^~<>!=]+\)|[a-zA-Z0-9_]+(?:\([^;\x0a?]*\)|(?:\s*(?:->|\.)\s*[a-zA-Z0-9_]+))*)\s*\?\s*:\s*([a-zA-Z0-9_]+\([^;\x0a()]*\)|[^,\);\x0a]+)'
    )

    def elvis_repl(m: re.Match[str]) -> str:
        if m.group(1) is not None:
            return m.group(1)
        return f"({m.group(2)} ? {m.group(2)} : {m.group(3)})"

    content = elvis_pattern.sub(elvis_repl, content)

    self_init_pattern = re.compile(
        r"\b((?:(?:const|unsigned|signed|struct|enum|long|short)\s+)*[a-zA-Z0-9_]+)(?:\s+(\*+)?|\s*(\*+))\s*([a-zA-Z0-9_]+)\s*=\s*\4\s*;"
    )

    def self_init_repl(m: re.Match[str]) -> str:
        prefix = m.group(1)
        stars = (m.group(2) or "") + (m.group(3) or "")
        var = m.group(4)
        if stars:
            return f"{prefix} {stars}{var} = 0;"
        return f"{prefix} {var} = 0;"

    content = self_init_pattern.sub(self_init_repl, content)

    content = content.replace(
        "# define UNINITIALIZED_VAR(var,zero) var = var",
        "# define UNINITIALIZED_VAR(var,zero) var = 0",
    )
    content = content.replace(
        "#if UINT_MAX == 0xffffffff" + NL + '# define PRIu32 "u"',
        '#ifndef PRIu32'
        + NL
        + '# define PRIu32 "u"'
        + NL
        + '# define PRId32 "d"'
        + NL
        + '# define PRIx32 "x"'
        + NL
        + '# define PRIX32 "X"'
        + NL
        + '# define PRIo32 "o"'
        + NL
        + '#endif'
        + NL
        + "#if UINT_MAX == 0xffffffff"
        + NL
        + '# define PRIu32 "u"',
    )
    content = content.replace(
        "#elif defined(__i386__)" + NL + "# define BB_BIG_ENDIAN 0",
        "#elif defined(__i386__) || defined(_WIN32) || defined(_MSC_VER)"
        + NL
        + "# define BB_BIG_ENDIAN 0",
    )
    content = content.replace(
        "#define get_unaligned_le32(buf) ({ uint32_t v; move_from_unaligned32(v, buf); SWAP_LE32(v); })"
        + NL
        + "#define get_unaligned_be32(buf) ({ uint32_t v; move_from_unaligned32(v, buf); SWAP_BE32(v); })",
        NL.join(
            [
                "static ALWAYS_INLINE uint32_t awm_bb_get_unaligned_le32(const void *buf) { uint32_t v; move_from_unaligned32(v, buf); return SWAP_LE32(v); }",
                "static ALWAYS_INLINE uint32_t awm_bb_get_unaligned_be32(const void *buf) { uint32_t v; move_from_unaligned32(v, buf); return SWAP_BE32(v); }",
                "#undef get_unaligned_le32",
                "#undef get_unaligned_be32",
                "#define get_unaligned_le32(buf) awm_bb_get_unaligned_le32(buf)",
                "#define get_unaligned_be32(buf) awm_bb_get_unaligned_be32(buf)",
            ]
        ),
    )
    content = content.replace(
        "#define get_le32(u32p) ({ uint32_t v = *(bb__aliased_uint32_t*)(u32p); SWAP_LE32(v); })",
        NL.join(
            [
                "static ALWAYS_INLINE uint32_t bb_get_le32(const void *u32p) { uint32_t v = *(const bb__aliased_uint32_t*)(u32p); return SWAP_LE32(v); }",
                "#define get_le32(u32p) bb_get_le32(u32p)",
            ]
        ),
    )
    content = content.replace(
        "#define isblank(a) ({ unsigned char bb__isblank = (a); bb__isblank == ' ' || bb__isblank == '"
        + BS
        + "t'; })"
        + NL
        + "#define iscntrl(a) ({ unsigned char bb__iscntrl = (a); bb__iscntrl < ' ' || bb__iscntrl == 0x7f; })",
        NL.join(
            [
                "static ALWAYS_INLINE int bb_ascii_isblank(unsigned char a) { return a == ' ' || a == '"
                + BS
                + "t'; }",
                "#define isblank(a) bb_ascii_isblank(a)",
                "static ALWAYS_INLINE int bb_ascii_iscntrl(unsigned char a) { return a < ' ' || a == 0x7f; }",
                "#define iscntrl(a) bb_ascii_iscntrl(a)",
            ]
        ),
    )
    content = content.replace(
        "#define isspace(a) ({ unsigned char bb__isspace = (a) - 9; bb__isspace == (' ' - 9) || bb__isspace <= (13 - 9); })",
        NL.join(
            [
                "static ALWAYS_INLINE int bb_ascii_isspace(unsigned char a) { unsigned char b = a - 9; return b == (' ' - 9) || b <= (13 - 9); }",
                "#define isspace(a) bb_ascii_isspace(a)",
            ]
        ),
    )
    xvfork_repl = NL.join(
        [
            "void bb_simple_perror_msg_and_die(const char *s) NORETURN FAST_FUNC;",
            "static ALWAYS_INLINE pid_t bb_xvfork(void) {",
            TAB + "pid_t bb__xvfork_pid = vfork();",
            TAB + "if (bb__xvfork_pid < 0)",
            TAB + TAB + 'bb_simple_perror_msg_and_die("vfork");',
            TAB + "return bb__xvfork_pid;",
            "}",
            "#define xvfork() bb_xvfork()",
        ]
    )
    content = content.replace(
        NL.join(
            [
                "#define xvfork() " + BS,
                "({",
                TAB + "pid_t bb__xvfork_pid = vfork(); " + BS,
                TAB + "if (bb__xvfork_pid < 0) " + BS,
                TAB + TAB + 'bb_simple_perror_msg_and_die("vfork"); ' + BS,
                TAB + "bb__xvfork_pid; " + BS,
                "})",
            ]
        ),
        xvfork_repl,
    )
    content = content.replace(
        NL.join(
            [
                "#define xvfork() " + BS,
                "({ " + BS,
                TAB + "pid_t bb__xvfork_pid = vfork(); " + BS,
                TAB + "if (bb__xvfork_pid < 0) " + BS,
                TAB + TAB + 'bb_simple_perror_msg_and_die("vfork"); ' + BS,
                TAB + "bb__xvfork_pid; " + BS,
                "})",
            ]
        ),
        xvfork_repl,
    )
    if "sigjmp_buf" in content and "typedef jmp_buf sigjmp_buf;" not in content:
        content = (
            NL.join(
                [
                    "#if defined(_WIN32)",
                    "#include <setjmp.h>",
                    "#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64))",
                    "static __inline int awm_clear_jmp_frame(_JBTYPE *env, int rc) { ((unsigned __int64 *)env)[0] = 0; return rc; }",
                    "#undef setjmp",
                    "#define setjmp(env) awm_clear_jmp_frame((env), _setjmp(env))",
                    "#endif",
                    "#ifndef _SIGJMP_BUF_DEFINED",
                    "#define _SIGJMP_BUF_DEFINED",
                    "typedef jmp_buf sigjmp_buf;",
                    "#endif",
                    "#ifndef sigsetjmp",
                    "#define sigsetjmp(env, savesigs) setjmp(env)",
                    "#endif",
                    "#ifndef siglongjmp",
                    "#define siglongjmp(env, val) longjmp((env), (val))",
                    "#endif",
                    "#endif",
                    "",
                ]
            )
            + content
        )
    content = content.replace(
        "static __inline pid_t bb_libbb_xvfork(void) {",
        "void bb_simple_perror_msg_and_die(const char *s) NORETURN FAST_FUNC;"
        + NL
        + "static __inline pid_t bb_libbb_xvfork(void) {",
    )
    content = content.replace(
        "#ifndef BB_GLOBAL_CONST" + NL + "# define BB_GLOBAL_CONST const" + NL + "#endif",
        NL.join(
            [
                "#ifndef BB_GLOBAL_CONST",
                "# if defined(_MSC_VER) || defined(_WIN32) || defined(__clang__)",
                "#  define BB_GLOBAL_CONST",
                "# else",
                "#  define BB_GLOBAL_CONST const",
                "# endif",
                "#endif",
            ]
        ),
    )
    content = content.replace(
        '#define barrier() asm volatile ("":::"memory")',
        NL.join(
            [
                "#ifdef _MSC_VER",
                "void _ReadWriteBarrier(void);",
                "#pragma intrinsic(_ReadWriteBarrier)",
                "#define barrier() _ReadWriteBarrier()",
                "#else",
                '#define barrier() asm volatile ("":::"memory")',
                "#endif",
            ]
        ),
    )

    content = _transform_packed_structs(content)

    content = content.replace(
        TAB + "TRUE, FALSE" + NL,
        NL.join(["#undef TRUE", "#undef FALSE", TAB + "TRUE, FALSE", ""]),
    )
    content = content.replace(
        TAB + "PIPE_BUF = 4096," + NL,
        NL.join(["#undef PIPE_BUF", TAB + "PIPE_BUF = 4096,", ""]),
    )
    content = content.replace(
        "#define MAXINT(T)",
        "#undef MAXINT" + NL + "#undef MININT" + NL + "#define MAXINT(T)",
    )
    content = content.replace(
        "void xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen) FAST_FUNC;",
        NL.join(
            [
                "void xconnect(int s, const struct sockaddr *sa_addr, socklen_t addrlen) FAST_FUNC;",
                "#if defined(_WIN32)",
                "#undef s_addr",
                "#define s_addr S_un.S_addr",
                "#endif",
                '#include "auto-win-msvc-configure.h"',
            ]
        ),
    )
    content = content.replace(
        "void FAST_FUNC xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen)",
        "#undef s_addr"
        + NL
        + "void FAST_FUNC xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen)",
    )
    content = content.replace(
        "extern char **environ;",
        "#ifndef environ" + NL + "extern char **environ;" + NL + "#endif",
    )
    content = content.replace(
        "#define getservbyport dont_use_getservbyport_uses_global_buffer",
        "#undef getservbyport"
        + NL
        + "#define getservbyport dont_use_getservbyport_uses_global_buffer",
    )
    content = content.replace(
        "#define getservbyname dont_use_getservbyname_uses_global_buffer",
        "#undef getservbyname"
        + NL
        + "#define getservbyname dont_use_getservbyname_uses_global_buffer",
    )
    content = content.replace(
        "loc = vstr = stackblock() + strloc;",
        "loc = vstr = (char *)stackblock() + strloc;",
    )
    content = content.replace(
        "typedef struct HFILE {",
        "#undef HFILE"
        + NL
        + "#define HFILE bb_HFILE"
        + NL
        + "typedef struct HFILE {",
    )
    content = content.replace(
        "# define fork()          BUG_fork_is_unavailable_on_nommu()",
        "# if !defined(_WIN32)"
        + NL
        + "#  define fork()          BUG_fork_is_unavailable_on_nommu()"
        + NL
        + "# endif",
    )
    content = content.replace(
        "if (ENABLE_FEATURE_SEAMLESS_Z"
        + NL
        + TAB
        + " && xstate->magic.b16[0] == COMPRESS_MAGIC"
        + NL
        + TAB
        + ") {"
        + NL
        + TAB
        + TAB
        + "xstate->xformer = unpack_Z_stream;"
        + NL
        + TAB
        + TAB
        + 'USE_FOR_NOMMU(xstate->xformer_prog = "uncompress";)'
        + NL
        + TAB
        + TAB
        + "goto found_magic;"
        + NL
        + TAB
        + "}",
        "#if ENABLE_FEATURE_SEAMLESS_Z"
        + NL
        + TAB
        + "if (xstate->magic.b16[0] == COMPRESS_MAGIC) {"
        + NL
        + TAB
        + TAB
        + "xstate->xformer = unpack_Z_stream;"
        + NL
        + TAB
        + TAB
        + 'USE_FOR_NOMMU(xstate->xformer_prog = "uncompress";)'
        + NL
        + TAB
        + TAB
        + "goto found_magic;"
        + NL
        + TAB
        + "}"
        + NL
        + "#endif",
    )
    content = content.replace(
        "const char *cp = strrchr(name, '/');",
        "#if defined(_WIN32) || defined(_MSC_VER)"
        + NL
        + TAB
        + "const char *cp_s1 = strrchr(name, '/');"
        + NL
        + TAB
        + "const char *cp_s2 = strrchr(name, '\\\\');"
        + NL
        + TAB
        + "const char *cp = (cp_s1 && cp_s2) ? (cp_s1 > cp_s2 ? cp_s1 : cp_s2) : (cp_s1 ? cp_s1 : cp_s2);"
        + NL
        + "#else"
        + NL
        + TAB
        + "const char *cp = strrchr(name, '/');"
        + NL
        + "#endif",
    )
    content = content.replace(
        "const char *slash = strrchr(path, '/');",
        "#if defined(_WIN32) || defined(_MSC_VER)"
        + NL
        + TAB
        + "const char *sl_s1 = strrchr(path, '/');"
        + NL
        + TAB
        + "const char *sl_s2 = strrchr(path, '\\\\');"
        + NL
        + TAB
        + "const char *slash = (sl_s1 && sl_s2) ? (sl_s1 > sl_s2 ? sl_s1 : sl_s2) : (sl_s1 ? sl_s1 : sl_s2);"
        + NL
        + "#else"
        + NL
        + TAB
        + "const char *slash = strrchr(path, '/');"
        + NL
        + "#endif",
    )
    content = content.replace(
        "pid_t FAST_FUNC spawn(char **argv)" + NL + "{",
        "pid_t FAST_FUNC spawn(char **argv)"
        + NL
        + "{"
        + NL
        + "#if defined(_WIN32)"
        + NL
        + TAB
        + "pid_t wpid = -1;"
        + NL
        + TAB
        + "int s_rc;"
        + NL
        + TAB
        + "fflush_all();"
        + NL
        + TAB
        + "s_rc = posix_spawnp(&wpid, argv[0], NULL, NULL, argv, environ);"
        + NL
        + TAB
        + "if (s_rc != 0) {"
        + NL
        + TAB
        + TAB
        + "char *bb_argv[64];"
        + NL
        + TAB
        + TAB
        + "char self_exe[512];"
        + NL
        + TAB
        + TAB
        + "int k = 0;"
        + NL
        + TAB
        + TAB
        + "/* GetModuleFileNameA declared via windows headers */"
        + NL
        + TAB
        + TAB
        + "if (GetModuleFileNameA(NULL, self_exe, sizeof(self_exe)) > 0) {"
        + NL
        + TAB
        + TAB
        + TAB
        + "bb_argv[0] = self_exe;"
        + NL
        + TAB
        + TAB
        + TAB
        + "while (argv[k] && k < 62) { bb_argv[k + 1] = argv[k]; k++; }"
        + NL
        + TAB
        + TAB
        + TAB
        + "bb_argv[k + 1] = NULL;"
        + NL
        + TAB
        + TAB
        + TAB
        + "s_rc = posix_spawnp(&wpid, self_exe, NULL, NULL, bb_argv, environ);"
        + NL
        + TAB
        + TAB
        + "}"
        + NL
        + TAB
        + "}"
        + NL
        + TAB
        + "if (s_rc != 0) { errno = s_rc; return -1; }"
        + NL
        + TAB
        + "return wpid;"
        + NL
        + "#endif",
    )
    content = content.replace(
        "#else"
        + NL
        + "void FAST_FUNC fork_transformer(int fd, const char *transform_prog)"
        + NL
        + "#endif"
        + NL
        + "{",
        "#else"
        + NL
        + "#if defined(_WIN32) && SEAMLESS_COMPRESSION"
        + NL
        + "static transformer_state_t *setup_transformer_on_fd(int fd, int die_if_not_compressed);"
        + NL
        + "#endif"
        + NL
        + "void FAST_FUNC fork_transformer(int fd, const char *transform_prog)"
        + NL
        + "#endif"
        + NL
        + "{"
        + NL
        + "#if defined(_WIN32)"
        + NL
        + TAB
        + "char tmp_name[64];"
        + NL
        + TAB
        + "static unsigned int s_seq = 0;"
        + NL
        + TAB
        + "int tmp_fd;"
        + NL
        + TAB
        + "transformer_state_t *xs = NULL;"
        + NL
        + TAB
        + "sprintf(tmp_name, \".bb_xfm_%u_%u.tmp\", (unsigned int)getpid(), ++s_seq);"
        + NL
        + TAB
        + "tmp_fd = open(tmp_name, O_RDWR | O_CREAT | O_TRUNC, 0600);"
        + NL
        + TAB
        + "if (tmp_fd < 0) bb_simple_perror_msg_and_die(\"open tmp\");"
        + NL
        + "#if SEAMLESS_COMPRESSION"
        + NL
        + TAB
        + "xs = setup_transformer_on_fd(fd, 0);"
        + NL
        + "#endif"
        + NL
        + TAB
        + "if (xs && xs->xformer) {"
        + NL
        + TAB
        + TAB
        + "xs->dst_fd = tmp_fd;"
        + NL
        + TAB
        + TAB
        + "xs->xformer(xs);"
        + NL
        + TAB
        + TAB
        + "free(xs);"
        + NL
        + TAB
        + TAB
        + "xlseek(tmp_fd, 0, SEEK_SET);"
        + NL
        + TAB
        + TAB
        + "xmove_fd(tmp_fd, fd);"
        + NL
        + TAB
        + "} else if (transform_prog && strcmp(transform_prog, \"unlzma\") == 0) {"
        + NL
        + TAB
        + TAB
        + "transformer_state_t lz;"
        + NL
        + TAB
        + TAB
        + "if (xs) free(xs);"
        + NL
        + TAB
        + TAB
        + "xlseek(fd, -2, SEEK_CUR);"
        + NL
        + TAB
        + TAB
        + "init_transformer_state(&lz);"
        + NL
        + TAB
        + TAB
        + "lz.src_fd = fd;"
        + NL
        + TAB
        + TAB
        + "lz.dst_fd = tmp_fd;"
        + NL
        + TAB
        + TAB
        + "unpack_lzma_stream(&lz);"
        + NL
        + TAB
        + TAB
        + "xlseek(tmp_fd, 0, SEEK_SET);"
        + NL
        + TAB
        + TAB
        + "xmove_fd(tmp_fd, fd);"
        + NL
        + TAB
        + "} else {"
        + NL
        + TAB
        + TAB
        + "if (xs) free(xs);"
        + NL
        + TAB
        + TAB
        + "close(tmp_fd);"
        + NL
        + TAB
        + "}"
        + NL
        + TAB
        + "unlink(tmp_name);"
        + NL
        + TAB
        + "return;"
        + NL
        + "#else",
    )
    content = content.replace(
        TAB + "/* parent process */" + NL + TAB + "close(fd_pipe.wr); /* don't want to write to the child */" + NL + TAB + "xmove_fd(fd_pipe.rd, fd);" + NL + "}",
        TAB + "/* parent process */" + NL + TAB + "close(fd_pipe.wr); /* don't want to write to the child */" + NL + TAB + "xmove_fd(fd_pipe.rd, fd);" + NL + "#endif" + NL + "}",
    )
    content = content.replace(
        "static void log_and_exit(void)" + NL + "{" + NL + TAB + "if (VERBOSE_3)" + NL + TAB + TAB + 'bb_simple_error_msg("closed");' + NL + TAB + "_exit_SUCCESS();" + NL + "}",
        "static void log_and_exit(void)"
        + NL
        + "{"
        + NL
        + TAB
        + "if (VERBOSE_3)"
        + NL
        + TAB
        + TAB
        + 'bb_simple_error_msg("closed");'
        + NL
        + "#if defined(_WIN32)"
        + NL
        + TAB
        + "if (die_func) die_func();"
        + NL
        + "#endif"
        + NL
        + TAB
        + "_exit_SUCCESS();"
        + NL
        + "}",
    )
    content = content.replace(
        "//send_headers_and_exit(HTTP_BAD_REQUEST);"
        + NL
        + TAB
        + TAB
        + "_exit_SUCCESS();",
        "//send_headers_and_exit(HTTP_BAD_REQUEST);"
        + NL
        + "#if defined(_WIN32)"
        + NL
        + TAB
        + TAB
        + "if (die_func) die_func();"
        + NL
        + "#endif"
        + NL
        + TAB
        + TAB
        + "_exit_SUCCESS();",
    )
    content = content.replace(
        "static void mini_httpd(int server_socket)" + NL + "{",
        "#if defined(_WIN32)"
        + NL
        + "static jmp_buf httpd_jmp;"
        + NL
        + "static void httpd_jump_back(void) { longjmp(httpd_jmp, 1); }"
        + NL
        + "#endif"
        + NL
        + "static void mini_httpd(int server_socket)"
        + NL
        + "{",
    )
    content = content.replace(
        "static void mini_httpd_nommu(int server_socket, int argc, char **argv)" + NL + "{",
        "#if defined(_WIN32)"
        + NL
        + "static jmp_buf httpd_jmp;"
        + NL
        + "static void httpd_jump_back(void) { longjmp(httpd_jmp, 1); }"
        + NL
        + "#endif"
        + NL
        + "static void mini_httpd_nommu(int server_socket, int argc, char **argv)"
        + NL
        + "{",
    )
    content = content.replace(
        "#if ULONG_MAX == 0xffffffffUL"
        + NL
        + "# define BC_NUM_LONG_LOG10 10"
        + NL
        + "#elif ULONG_MAX == 0xffffffffffffffffULL",
        "#if SIZE_MAX == 0xffffffffUL"
        + NL
        + "# define BC_NUM_LONG_LOG10 10"
        + NL
        + "#elif SIZE_MAX == 0xffffffffffffffffULL",
    )
    content = content.replace(
        "#if ENABLE_PING6"
        + NL
        + TAB
        + "while ((++argv)[0] && argv[0][0] == '-') {"
        + NL
        + TAB
        + TAB
        + "if (argv[0][1] == '4') {"
        + NL
        + TAB
        + TAB
        + TAB
        + "af = AF_INET;"
        + NL
        + TAB
        + TAB
        + TAB
        + "continue;"
        + NL
        + TAB
        + TAB
        + "}"
        + NL
        + TAB
        + TAB
        + "if (argv[0][1] == '6') {"
        + NL
        + TAB
        + TAB
        + TAB
        + "af = AF_INET6;"
        + NL
        + TAB
        + TAB
        + TAB
        + "continue;"
        + NL
        + TAB
        + TAB
        + "}"
        + NL
        + TAB
        + TAB
        + "bb_show_usage();"
        + NL
        + TAB
        + "}"
        + NL
        + "#else"
        + NL
        + TAB
        + "argv++;"
        + NL
        + "#endif",
        "while ((++argv)[0] && argv[0][0] == '-') {"
        + NL
        + "#if ENABLE_PING6"
        + NL
        + TAB
        + TAB
        + "if (argv[0][1] == '4') { af = AF_INET; continue; }"
        + NL
        + TAB
        + TAB
        + "if (argv[0][1] == '6') { af = AF_INET6; continue; }"
        + NL
        + "#endif"
        + NL
        + TAB
        + TAB
        + "if (argv[0][1] == 'c' || argv[0][1] == 'W' || argv[0][1] == 'w' || argv[0][1] == 's' || argv[0][1] == 't' || argv[0][1] == 'I') {"
        + NL
        + TAB
        + TAB
        + TAB
        + "if (!argv[0][2] && argv[1]) argv++;"
        + NL
        + TAB
        + TAB
        + TAB
        + "continue;"
        + NL
        + TAB
        + TAB
        + "}"
        + NL
        + TAB
        + TAB
        + "if (argv[0][1] == 'q' || argv[0][1] == 'v') continue;"
        + NL
        + TAB
        + TAB
        + "bb_show_usage();"
        + NL
        + TAB
        + "}",
    )
    content = content.replace(
        "xmove_fd(server_socket, 0);"
        + NL
        + TAB
        + "/* NB: it's best to not use xfuncs in this loop before vfork().",
        "#if !defined(_WIN32)"
        + NL
        + TAB
        + "xmove_fd(server_socket, 0);"
        + NL
        + "#endif"
        + NL
        + TAB
        + "/* NB: it's best to not use xfuncs in this loop before vfork().",
    )
    content = content.replace(
        "xmove_fd(server_socket, 0);" + NL + TAB + "/* NB: it's best to not use xfuncs",
        "#if !defined(_WIN32)"
        + NL
        + TAB
        + "xmove_fd(server_socket, 0);"
        + NL
        + "#endif"
        + NL
        + TAB
        + "/* NB: it's best to not use xfuncs",
    )
    content = content.replace(
        "countdown = G.conn_limit;" + NL + TAB + "while (1) {",
        "countdown = G.conn_limit;"
        + NL
        + TAB
        + "while (1) {"
        + NL
        + "#if defined(_WIN32)"
        + NL
        + TAB
        + TAB
        + "int s;"
        + NL
        + TAB
        + TAB
        + "len_and_sockaddr fromAddr;"
        + NL
        + TAB
        + TAB
        + "fromAddr.len = LSA_SIZEOF_SA;"
        + NL
        + TAB
        + TAB
        + "s = accept(server_socket, &fromAddr.u.sa, &fromAddr.len);"
        + NL
        + TAB
        + TAB
        + "if (s < 0) continue;"
        + NL
        + TAB
        + TAB
        + "xdup2(s, 0); xdup2(s, 1);"
        + NL
        + TAB
        + TAB
        + "if (s > 1) close(s);"
        + NL
        + TAB
        + TAB
        + "hdr_cnt = 0;"
        + NL
        + TAB
        + TAB
        + "if (setjmp(httpd_jmp) == 0) {"
        + NL
        + TAB
        + TAB
        + TAB
        + "die_func = httpd_jump_back;"
        + NL
        + TAB
        + TAB
        + TAB
        + "handle_incoming_and_exit(&fromAddr);"
        + NL
        + TAB
        + TAB
        + "}"
        + NL
        + TAB
        + TAB
        + "die_func = NULL;"
        + NL
        + TAB
        + TAB
        + "fflush_all();"
        + NL
        + TAB
        + TAB
        + "close(0); close(1);"
        + NL
        + TAB
        + TAB
        + "continue;"
        + NL
        + "#endif",
    )

    content = content.replace(
        "#if !BB_MMU"
        + NL
        + '# error "Do not even bother, ash will not run on NOMMU machine"',
        "#if !BB_MMU && !defined(_WIN32) && !defined(_MSC_VER)"
        + NL
        + '# error "Do not even bother, ash will not run on NOMMU machine"',
    )
    content = content.replace(
        "#if BUILD_AS_NOMMU" + NL,
        "#if BUILD_AS_NOMMU || defined(_WIN32)" + NL,
    )
    content = content.replace(
        "#define ISSPACE(a) ({ unsigned char xargs__isspace = (a) - 9; xargs__isspace == (' ' - 9) || xargs__isspace <= (13 - 9); })",
        "#define ISSPACE(a) (isspace((unsigned char)(a)))",
    )
    content = content.replace(
        "static void printf_s(char *pformat, const char *msg)",
        "#undef printf_s"
        + NL
        + "#define printf_s bb_local_printf_s"
        + NL
        + "static void bb_local_printf_s(char *pformat, const char *msg)",
    )
    content = content.replace(
        "} L = L; /* for compiler */",
        "} L; memset(&L, 0, sizeof(L));",
    )
    content = content.replace(
        "} R = R;",
        "} R; memset(&R, 0, sizeof(R));",
    )
    content = content.replace(
        "# error Not implemented for this value of RAND_MAX",
        "uint32_t u = ((uint32_t)rand() << 16) | (uint32_t)rand();"
        + NL
        + TAB
        + TAB
        + TAB
        + TAB
        + "R_d = (double)u / 4294967296.0;",
    )
    content = content.replace(
        "awk_exit();" + NL + TAB + "/*return 0;*/",
        "awk_exit();" + NL + TAB + "return 0;",
    )
    content = content.replace(
        "fflush_stdout_and_exit_SUCCESS();" + NL + "}" + NL + "#endif",
        "fflush_stdout_and_exit_SUCCESS();"
        + NL
        + TAB
        + "return 0;"
        + NL
        + "}"
        + NL
        + "#endif",
    )
    content = content.replace(
        "#else" + NL + "void sed_free_and_close_stuff(void);" + NL + "#endif",
        "#else"
        + NL
        + "static void sed_free_and_close_stuff(void) {}"
        + NL
        + "#endif",
    )
    content = content.replace(
        "#if ENABLE_FEATURE_CLEAN_UP"
        + NL
        + "/* Clear statbuf hash table */"
        + NL
        + "void FAST_FUNC reset_ino_dev_hashtable(void)",
        "#if 1 /* ENABLE_FEATURE_CLEAN_UP */"
        + NL
        + "/* Clear statbuf hash table */"
        + NL
        + "void FAST_FUNC reset_ino_dev_hashtable(void)",
    )
    content = content.replace(
        "char *chp, *opt_t;",
        "char *chp = NULL, *opt_t = NULL;",
    )
    content = content.replace(
        "&& st.st_size > (off_t)0xffffffff",
        "&& st.st_size > 0xffffffffLL",
    )
    content = content.replace(
        "if (IS_BC || (option_mask32 & BC_FLAG_I))",
        "if (!G_exiting && (IS_BC || (option_mask32 & BC_FLAG_I)))",
    )
    content = content.replace(
        TAB
        + "pid = xvfork();"
        + NL
        + TAB
        + "if (pid == 0) {"
        + NL
        + TAB
        + TAB
        + "/* Child */"
        + NL
        + TAB
        + TAB
        + "BB_EXECVP_or_die((char**)cmd);"
        + NL
        + TAB
        + "}",
        TAB + "pid = spawn((char**)cmd);",
    )
    content = content.replace(
        "enum size_spec {" + NL + TAB + "NO_SIZE,",
        "#undef CHAR"
        + NL
        + "#undef SHORT"
        + NL
        + "#undef INT"
        + NL
        + "#undef LONG"
        + NL
        + "#define CHAR BB_ENUM_CHAR"
        + NL
        + "#define SHORT BB_ENUM_SHORT"
        + NL
        + "#define INT BB_ENUM_INT"
        + NL
        + "#define LONG BB_ENUM_LONG"
        + NL
        + "enum size_spec {"
        + NL
        + TAB
        + "NO_SIZE,",
    )
    content = content.replace(
        "#if defined(__clang_major__) && __clang_major__ >= 9",
        "#if defined(__clang_major__) && __clang_major__ >= 9 && !defined(_MSC_VER)",
    )
    content = content.replace(
        "#define NORETURN __attribute__ ((__noreturn__))",
        "#ifdef _MSC_VER"
        + NL
        + "# define NORETURN __declspec(noreturn)"
        + NL
        + "#else"
        + NL
        + "# define NORETURN __attribute__ ((__noreturn__))"
        + NL
        + "#endif",
    )
    content = content.replace(
        "void bb_error_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;",
        "NORETURN void bb_error_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;",
    )
    content = content.replace(
        "void bb_perror_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;",
        "NORETURN void bb_perror_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;",
    )
    content = content.replace(
        "void bb_herror_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;",
        "NORETURN void bb_herror_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;",
    )
    content = content.replace(
        "#define OC_B   OC_BUILTIN" + NL + "#define OC_F   OC_FBLTIN",
        "#define OC_B   ((int)OC_BUILTIN)" + NL + "#define OC_F   ((int)OC_FBLTIN)",
    )
    content = content.replace(
        "#define xstrtou(rest) xstrtoull##rest"
        + NL
        + "#define xstrto(rest) xstrtoll##rest"
        + NL
        + "#define xatou(rest) xatoull##rest"
        + NL
        + "#define xato(rest) xatoll##rest",
        "#define xstrtou(rest) xstrtoull##rest"
        + NL
        + "#define xstrtou_plain xstrtoull"
        + NL
        + "#define xstrto(rest) xstrtoll##rest"
        + NL
        + "#define xstrto_plain xstrtoll"
        + NL
        + "#define xatou(rest) xatoull##rest"
        + NL
        + "#define xatou_plain xatoull"
        + NL
        + "#define xato(rest) xatoll##rest"
        + NL
        + "#define xato_plain xatoll",
    )
    content = content.replace(
        "#define xstrtou(rest) xstrtoul##rest"
        + NL
        + "#define xstrto(rest) xstrtol##rest"
        + NL
        + "#define xatou(rest) xatoul##rest"
        + NL
        + "#define xato(rest) xatol##rest",
        "#define xstrtou(rest) xstrtoul##rest"
        + NL
        + "#define xstrtou_plain xstrtoul"
        + NL
        + "#define xstrto(rest) xstrtol##rest"
        + NL
        + "#define xstrto_plain xstrtol"
        + NL
        + "#define xatou(rest) xatoul##rest"
        + NL
        + "#define xatou_plain xatoul"
        + NL
        + "#define xato(rest) xatol##rest"
        + NL
        + "#define xato_plain xatol",
    )
    content = content.replace(
        "#define xstrtou(rest) xstrtou##rest"
        + NL
        + "#define xstrto(rest) xstrtoi##rest"
        + NL
        + "#define xatou(rest) xatou##rest"
        + NL
        + "#define xato(rest) xatoi##rest",
        "#define xstrtou(rest) xstrtou##rest"
        + NL
        + "#define xstrtou_plain xstrtou"
        + NL
        + "#define xstrto(rest) xstrtoi##rest"
        + NL
        + "#define xstrto_plain xstrtoi"
        + NL
        + "#define xatou(rest) xatou##rest"
        + NL
        + "#define xatou_plain xatou"
        + NL
        + "#define xato(rest) xatoi##rest"
        + NL
        + "#define xato_plain xatoi",
    )
    content = content.replace(
        "unsigned type FAST_FUNC xstrtou()(const char *numstr, int base)",
        "unsigned type FAST_FUNC xstrtou_plain(const char *numstr, int base)",
    )
    content = content.replace(
        "unsigned type FAST_FUNC xatou()(const char *numstr)",
        "unsigned type FAST_FUNC xatou_plain(const char *numstr)",
    )
    content = content.replace(
        "type FAST_FUNC xstrto()(const char *numstr, int base)",
        "type FAST_FUNC xstrto_plain(const char *numstr, int base)",
    )
    content = content.replace(
        "type FAST_FUNC xato()(const char *numstr)",
        "type FAST_FUNC xato_plain(const char *numstr)",
    )
    content = content.replace(
        "#undef xato" + NL,
        "#undef xato"
        + NL
        + "#undef xstrtou_plain"
        + NL
        + "#undef xstrto_plain"
        + NL
        + "#undef xatou_plain"
        + NL
        + "#undef xato_plain"
        + NL,
    )
    content = content.replace(
        "static void abort_unzip(STATE_PARAM_ONLY) NORETURN;"
        + NL
        + "static void abort_unzip(STATE_PARAM_ONLY)",
        "static NORETURN void abort_unzip(STATE_PARAM_ONLY)"
        + NL
        + "{"
        + NL
        + TAB
        + "huft_free_all(PASS_STATE_ONLY);"
        + NL
        + TAB
        + "longjmp(error_jmp, 1);"
        + NL
        + "}"
        + NL
        + "#if 0",
    )
    content = content.replace(
        "#if 0"
        + NL
        + "{"
        + NL
        + TAB
        + "huft_free_all(PASS_STATE_ONLY);"
        + NL
        + TAB
        + "longjmp(error_jmp, 1);"
        + NL
        + "}",
        "",
    )
    content = re.sub(
        r"(^[ \t]*)((?:(?:extern|static|inline|ALWAYS_INLINE|NOINLINE|FAST_FUNC)\s+)*(?:const\s+)?(?:struct\s+\w+\s*\*+\s*|void|int|char\s*\*+)\s*[a-zA-Z0-9_]+\s*\([^;{}]*\))\s*NORETURN(\s+FAST_FUNC)?\s*;",
        lambda m: f"{m.group(1)}NORETURN {m.group(2)}{m.group(3) or ''};",
        content,
        flags=re.MULTILINE,
    )
    content = content.replace(
        "#if ULONG_MAX > 0xffffffff"
        + NL
        + TAB
        + "// 64-bit version (will not work correctly for 32-bit longs!)"
        + NL
        + TAB
        + "return BC_PARSE_EXPRS_BITS & (1UL << i);",
        "#if SIZE_MAX > 0xffffffff"
        + NL
        + TAB
        + "return (BC_PARSE_EXPRS_BITS & (1ULL << i)) != 0;",
    )
    content = content.replace(
        "unsigned long m = (uint32_t)BC_PARSE_EXPRS_BITS;"
        + NL
        + TAB
        + "if (i >= 32) {"
        + NL
        + TAB
        + TAB
        + "m = (uint32_t)(BC_PARSE_EXPRS_BITS >> 32);",
        "unsigned long m = (uint32_t)BC_PARSE_EXPRS_BITS;"
        + NL
        + TAB
        + "if (i >= 32) {"
        + NL
        + TAB
        + TAB
        + "m = (uint32_t)((uint64_t)BC_PARSE_EXPRS_BITS >> 32);",
    )
    content = content.replace(
        "unsigned long memleak_value;",
        "uintptr_t memleak_value;",
    )
    content = content.replace(
        "void *p;" + NL + TAB + "unsigned long l;",
        "void *p;" + NL + TAB + "uintptr_t l;",
    )
    content = content.replace(
        "l = (unsigned long)p;",
        "l = (uintptr_t)p;",
    )
    content = content.replace(
        "if (l < (unsigned long)p)",
        "if (l < (uintptr_t)p)",
    )
    content = content.replace(
        "if ((long)l < 0)",
        "if ((intptr_t)l < 0)",
    )
    content = content.replace(
        "len_and_sockaddr* FAST_FUNC get_sock_lsa(int fd)"
        + NL
        + "{"
        + NL
        + TAB
        + "return get_lsa(fd, getsockname);"
        + NL
        + "}"
        + NL
        + NL
        + "len_and_sockaddr* FAST_FUNC get_peer_lsa(int fd)"
        + NL
        + "{"
        + NL
        + TAB
        + "return get_lsa(fd, getpeername);"
        + NL
        + "}",
        "len_and_sockaddr* FAST_FUNC get_sock_lsa(int fd)"
        + NL
        + "{"
        + NL
        + TAB
        + "return get_lsa(fd, (int (*)(int, struct sockaddr *, socklen_t *))getsockname);"
        + NL
        + "}"
        + NL
        + NL
        + "len_and_sockaddr* FAST_FUNC get_peer_lsa(int fd)"
        + NL
        + "{"
        + NL
        + TAB
        + "return get_lsa(fd, (int (*)(int, struct sockaddr *, socklen_t *))getpeername);"
        + NL
        + "}",
    )
    content = content.replace(
        "typedef BC_STATUS (*BcNumBinaryOp)(BcNum *, BcNum *, BcNum *, size_t) FAST_FUNC;",
        "typedef BC_STATUS (*BcNumBinaryOp)(BcNum *, BcNum *, BcNum *restrict, size_t) FAST_FUNC;",
    )
    content = re.sub(
        r"(static FAST_FUNC BC_STATUS zbc_num_(?:add|sub|mul|div|mod|pow)\(BcNum \*a, BcNum \*b, BcNum \*)c(\s*, size_t scale\))",
        r"\1restrict c\2",
        content,
    )
    content = content.replace(
        "#define ENABLE_HUSH_DOLLAR_OPS 1",
        "#undef ENABLE_HUSH_DOLLAR_OPS" + NL + "#define ENABLE_HUSH_DOLLAR_OPS 1",
    )
    content = content.replace(
        "# define IF_HAS_KEYWORDS(...) __VA_ARGS__",
        "# undef IF_HAS_KEYWORDS" + NL + "# define IF_HAS_KEYWORDS(...) __VA_ARGS__",
    )
    content = content.replace(
        "# define IF_HAS_KEYWORDS(...)",
        "# undef IF_HAS_KEYWORDS" + NL + "# define IF_HAS_KEYWORDS(...)",
    )
    for bash_sym in (
        "IF_BASH_FUNCTION",
        "IF_BASH_REDIR_OUTPUT",
        "IF_BASH_DOLLAR_SQUOTE",
        "IF_BASH_PATTERN_SUBST",
        "IF_BASH_SUBSTR",
        "IF_BASH_READ_D",
        "IF_BASH_PROCESS_SUBST",
    ):
        content = content.replace(
            f"#define {bash_sym} ",
            f"#undef {bash_sym}" + NL + f"#define {bash_sym} ",
        )
        content = content.replace(
            f"# define {bash_sym}",
            f"# undef {bash_sym}" + NL + f"# define {bash_sym}",
        )

    return content


def transform_file(input_path: str, output_path: str) -> None:
    """
    Read an input C source/header file, apply C89/MSVC transformations,
    rewrite relative local includes if needed, and write to output_path.

    Args:
        input_path: Path to source file.
        output_path: Destination path for transformed file.
    """
    abs_in = os.path.abspath(input_path)
    in_dir = os.path.dirname(abs_in)
    with open(abs_in, "r", encoding="utf-8", errors="ignore") as f:
        original = f.read()

    def resolve_local_include(match: re.Match[str]) -> str:
        inc_rel = match.group(1)
        candidate = os.path.normpath(os.path.join(in_dir, inc_rel))
        if os.path.isfile(candidate):
            with open(candidate, "r", encoding="utf-8", errors="ignore") as cf:
                c_text = cf.read()
            c_trans = transform_source_content(c_text)
            if c_trans != c_text:
                out_dir_abs = os.path.dirname(os.path.abspath(output_path))
                gen_inc = os.path.join(out_dir_abs, inc_rel)
                os.makedirs(os.path.dirname(gen_inc), exist_ok=True)
                with open(gen_inc, "w", encoding="utf-8") as gf:
                    gf.write(c_trans)
                return f'#include "{gen_inc.replace(BS, "/")}"'
            return f'#include "{candidate.replace(BS, "/")}"'
        return match.group(0)

    if abs_in.endswith(".c"):
        original = re.sub(
            r'#\s*include\s+"([^"]+)"', resolve_local_include, original
        )

    transformed = transform_source_content(original)

    out_dir = os.path.dirname(os.path.abspath(output_path))
    os.makedirs(out_dir, exist_ok=True)

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(transformed)


def main() -> None:
    """
    CLI entry point for transform_sources.py.
    """
    parser = argparse.ArgumentParser(
        description="Transform C source files for MSVC compatibility."
    )
    parser.add_argument(
        "inputs", nargs="+", help="Input file(s) or input/output pair"
    )
    parser.add_argument(
        "--output", "-o", help="Output file path (for single input)"
    )
    parser.add_argument(
        "--out-dir", "-d", help="Output directory for generated sources"
    )

    args = parser.parse_args()

    if args.output:
        if len(args.inputs) != 1:
            print("Error: --output can only be used with a single input file.")
            sys.exit(1)
        transform_file(args.inputs[0], args.output)
    elif args.out_dir:
        for inp in args.inputs:
            rel_name = os.path.basename(inp)
            out_file = os.path.join(args.out_dir, rel_name)
            transform_file(inp, out_file)
    elif len(args.inputs) == 2 and not os.path.isdir(args.inputs[1]):
        transform_file(args.inputs[0], args.inputs[1])
    else:
        print(
            "Usage: transform_sources.py <input> <output> OR --out-dir <dir> <files...>"
        )
        sys.exit(1)


if __name__ == "__main__":  # pragma: no cover
    main()
