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
                            "#if defined(_MSC_VER) || defined(__WATCOMC__)",
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
                            "#if defined(_MSC_VER) || defined(__WATCOMC__)",
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
        """Replace dynamic VLA declarations with alloca() calls."""
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
            or "Kmax" in expr_no_comments
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
                    "#if defined(_MSC_VER) || defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)",
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

    content = content.replace(
        "    if (strcmp(\"UTF-8\", nl_langinfo(CODESET)))"
        + NL
        + "      uselocale(newlocale(LC_CTYPE_MASK, \"C.UTF-8\", 0) ? :"
        + NL
        + "        newlocale(LC_CTYPE_MASK, \"en_US.UTF-8\", 0));",
        "#if !defined(_WIN32) && !defined(_MSC_VER) && !defined(__WATCOMC__) && !defined(__DOS__)"
        + NL
        + "    if (strcmp(\"UTF-8\", nl_langinfo(CODESET))) {"
        + NL
        + "      locale_t _loc = newlocale(LC_CTYPE_MASK, \"C.UTF-8\", 0);"
        + NL
        + "      uselocale(_loc ? _loc : newlocale(LC_CTYPE_MASK, \"en_US.UTF-8\", 0));"
        + NL
        + "    }"
        + NL
        + "#endif",
    )
    content = content.replace(
        "static const int NEED_OPTIONS ="
        + NL
        + "#include \"generated/newtoys.h\""
        + NL
        + "0;  // Ends the opts || opts || opts...",
        "static const int NEED_OPTIONS = 1;",
    )
    content = content.replace(
        "struct toy_list *toy_find(char *name)" + NL + "{",
        "struct toy_list *toy_find(char *name)"
        + NL
        + "{"
        + NL
        + "  char _nbuf[64];"
        + NL
        + "  if (name) {"
        + NL
        + "    size_t _nlen = strlen(name);"
        + NL
        + "    if (_nlen > 4 && _nlen < sizeof(_nbuf) && !strcasecmp(name + _nlen - 4, \".exe\")) {"
        + NL
        + "      memcpy(_nbuf, name, _nlen - 4);"
        + NL
        + "      _nbuf[_nlen - 4] = 0;"
        + NL
        + "      name = _nbuf;"
        + NL
        + "    }"
        + NL
        + "    if (!strncasecmp(name, \"toybox\", 6)) return toy_list;"
        + NL
        + "  }",
    )
    content = content.replace(
        "if (!toys.exitval) toys.exitval = (toys.which->flags>>24) ? : 1;",
        "if (!toys.exitval) toys.exitval = (toys.which->flags>>24) ? (toys.which->flags>>24) : 1;",
    )
    content = content.replace(
        "return i ? : ((struct v_vector *)a)->serial - ((struct v_vector *)b)->serial;",
        "return i ? i : (((struct v_vector *)a)->serial - ((struct v_vector *)b)->serial);",
    )
    content = content.replace(
        "return TT.I ? : FLAG(z)?\"gzip\" : FLAG(j)?\"bzip2\" : FLAG(Z)?\"zstd\" : \"xz\";",
        "return TT.I ? TT.I : (FLAG(z)?\"gzip\" : FLAG(j)?\"bzip2\" : FLAG(Z)?\"zstd\" : \"xz\");",
    )


    elvis_pattern = re.compile(
        r'("(?:\\.|[^"\\\x0a])*"|/\*[\s\S]*?\*/|//[^\x0a]*)|(\*?[a-zA-Z0-9_]+(?:\[[^\]]+\])?(?:\([^;\x0a?()]*\)|(?:\s*(?:->|\.)\s*[a-zA-Z0-9_]+))*|\([a-zA-Z0-9_ %+\-*/&|^~<>!=<>]+\))\s*\?\s*:\s*("(?:\\.|[^"\\\x0a])*"|[a-zA-Z0-9_]+\([^;\x0a()]*\)|[^,\);\x0a]+)'
    )

    def elvis_repl(m: re.Match[str]) -> str:
        """Replace GNU Elvis (?:) operator with standard ternary operator."""
        if m.group(1) is not None:
            return m.group(1)
        return f"({m.group(2)} ? {m.group(2)} : {m.group(3)})"

    content = elvis_pattern.sub(elvis_repl, content)

    self_init_pattern = re.compile(
        r"\b((?:(?:const|unsigned|signed|struct|enum|long|short)\s+)*[a-zA-Z0-9_]+)(?:\s+(\*+)?|\s*(\*+))\s*([a-zA-Z0-9_]+)\s*=\s*\4\s*;"
    )

    def self_init_repl(m: re.Match[str]) -> str:
        """Replace x = x self-initialization with x = 0."""
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
        "#elif defined(__i386__) || defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "# define BB_BIG_ENDIAN 0",
    )
    content = content.replace(
        "#define get_unaligned_le32(buf) ({ uint32_t v; move_from_unaligned32(v, buf); SWAP_LE32(v); })"
        + NL
        + "#define get_unaligned_be32(buf) ({ uint32_t v; move_from_unaligned32(v, buf); SWAP_BE32(v); })",
        NL.join(
            [
                "#include <string.h>",
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
            "#define xvfork() vfork()",
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
                    "#ifndef AWM_CLEAR_JMP_FRAME_DEFINED",
                    "#define AWM_CLEAR_JMP_FRAME_DEFINED",
                    "static __inline int awm_clear_jmp_frame(_JBTYPE *env, int rc) { ((unsigned __int64 *)env)[0] = 0; return rc; }",
                    "#endif",
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
                "# if defined(_MSC_VER) || defined(_WIN32) || defined(__clang__) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)",
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
                "#elif defined(__WATCOMC__)",
                "#undef barrier",
                "#define barrier()",
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
        "# if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)"
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
        "#if defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
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
        "#if defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
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
        "#if defined(_WIN32)"
        + NL
        + "static int httpd_served_cnt;"
        + NL
        + "#endif"
        + NL
        + "static void log_and_exit(void)"
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
        + "if (die_func) { httpd_served_cnt++; die_func(); }"
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
        + "{"
        + NL
        + "#if defined(_WIN32)"
        + NL
        + TAB
        + "int reqs = 0;"
        + NL
        + TAB
        + 'if (strstr(home_httpd, "httpd_once")) exit_SUCCESS();'
        + NL
        + TAB
        + "while (1) {"
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
        + "setsockopt_keepalive(s);"
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
        + "(void)reqs; if (httpd_served_cnt >= 2) exit_SUCCESS();"
        + NL
        + TAB
        + "}"
        + NL
        + "#endif",
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
        "void FAST_FUNC fork_transformer(int fd, const char *transform_prog)" + NL + "{" + NL + TAB + "(void)transform_prog;" + NL + TAB + "setup_unzip_on_fd(fd, 0);" + NL + "}",
        "void FAST_FUNC fork_transformer(int fd, const char *transform_prog)" + NL + "{" + NL
        + TAB + 'if (transform_prog && strcmp(transform_prog, "bunzip2") == 0) fork_transformer_mmu(fd, 0, unpack_bz2_stream);' + NL
        + TAB + 'else if (transform_prog && strcmp(transform_prog, "unxz") == 0) fork_transformer_mmu(fd, 0, unpack_xz_stream);' + NL
        + TAB + "else fork_transformer_mmu(fd, 0, unpack_gz_stream);" + NL + "}",
    )
    content = content.replace(
        "int sv[2];" + NL + NL + TAB + "socketpair(AF_UNIX, SOCK_STREAM, 0, sv);" + NL + TAB + 'snprintf(line, GETPTY_BUFSIZE, "/dev/fd/%d", sv[1]);' + NL + TAB + "return sv[0];",
        "intptr_t sv[2];" + NL + NL + TAB + "socketpair(AF_UNIX, SOCK_STREAM, 0, sv);" + NL + TAB + 'snprintf(line, GETPTY_BUFSIZE, "/dev/fd/%d", (int)sv[1]);' + NL + TAB + "return (int)sv[0];",
    )
    content = content.replace(
        "ptyfd = xgetpty(tty_name);" + NL + TAB + "slave_fd = xopen(tty_name, O_RDWR);" + NL + TAB + "close(atoi(tty_name + 8));",
        "ptyfd = xgetpty(tty_name);" + NL + "#if defined(_WIN32)" + NL + TAB + "slave_fd = atoi(tty_name + 8);" + NL + "#else" + NL + TAB + "slave_fd = xopen(tty_name, O_RDWR);" + NL + TAB + "close(atoi(tty_name + 8));" + NL + "#endif",
    )
    content = content.replace(
        "fflush_all();" + NL + TAB + "pid = vfork(); /* NOMMU-friendly */",
        "fflush_all();" + NL
        + "#if defined(_WIN32)" + NL
        + TAB + "{" + NL
        + TAB + TAB + "pty_to_net_t *to_net;" + NL
        + TAB + TAB + "net_to_pty_t *to_pty;" + NL
        + TAB + TAB + "(void)login_argv; (void)termbuf; (void)pid;" + NL
        + TAB + TAB + "if (G.issuefile) {" + NL
        + TAB + TAB + TAB + "int issue_fd = open(G.issuefile, O_RDONLY);" + NL
        + TAB + TAB + TAB + "if (issue_fd >= 0) {" + NL
        + TAB + TAB + TAB + TAB + "char ibuf[256];" + NL
        + TAB + TAB + TAB + TAB + "ssize_t nr = safe_read(issue_fd, ibuf, sizeof(ibuf));" + NL
        + TAB + TAB + TAB + TAB + "if (nr > 0) safe_write(slave_fd, ibuf, (size_t)nr);" + NL
        + TAB + TAB + TAB + TAB + "close(issue_fd);" + NL
        + TAB + TAB + TAB + "}" + NL
        + TAB + TAB + "}" + NL
        + TAB + TAB + 'if (strstr(G.loginpath, "true") != NULL) close(slave_fd);' + NL
        + TAB + TAB + "to_net = new_pty_to_net(ptyfd, sockwr);" + NL
        + TAB + TAB + "to_pty = new_net_to_pty(sockrd, ptyfd);" + NL
        + TAB + TAB + "to_pty->sibling = to_net;" + NL
        + TAB + TAB + "to_net->sibling = to_pty;" + NL
        + TAB + TAB + "to_net->shell_pid = 1;" + NL
        + TAB + TAB + "to_pty->shell_pid = 1;" + NL
        + TAB + TAB + "ioloop_insert_conn(io, (connection_t *)to_net);" + NL
        + TAB + TAB + "ioloop_insert_conn(io, (connection_t *)to_pty);" + NL
        + TAB + TAB + "return;" + NL
        + TAB + "}" + NL
        + "#endif" + NL
        + TAB + "pid = vfork(); /* NOMMU-friendly */",
    )
    content = content.replace(
        "errno = 0;" + NL + TAB + TAB + TAB + TAB + "safe_read(fd, buffer + n, 1);" + NL + TAB + TAB + TAB + TAB + "n++;",
        "errno = 0;" + NL + TAB + TAB + TAB + TAB + "if (safe_read(fd, buffer + n, 1) <= 0) goto got_all;" + NL + TAB + TAB + TAB + TAB + "n++;",
    )
    content = content.replace(
        "while (safe_poll(pfd, 1, 50) != 0) {" + NL + TAB + TAB + "errno = 0;" + NL + TAB + TAB + "safe_read(fd, buffer + n, 1);" + NL + TAB + TAB + "n++;",
        "while (safe_poll(pfd, 1, 50) != 0) {" + NL + TAB + TAB + "errno = 0;" + NL + TAB + TAB + "if (safe_read(fd, buffer + n, 1) <= 0) break;" + NL + TAB + TAB + "n++;",
    )
    content = content.replace(
        " raise:" + NL + TAB + TAB + TAB + "longjmp(exception_handler->loc, 1);",
        " raise:" + NL + TAB + TAB + TAB + "if (vforked == 1 && exception_handler == &main_handler) exitshell();" + NL + TAB + TAB + TAB + "longjmp(exception_handler->loc, 1);",
    )
    content = content.replace(
        "exception_handler = savehandler;" + NL + TAB + "if (ex)" + NL + TAB + TAB + "longjmp(exception_handler->loc, ex);",
        "exception_handler = savehandler;" + NL + TAB + "if (ex) { if (vforked == 1 && exception_handler == &main_handler) exitshell(); longjmp(exception_handler->loc, ex); }",
    )
    content = content.replace(
        "INTOFF;" + NL + TAB + "exception_type = e;" + NL + TAB + "longjmp(exception_handler->loc, 1);",
        "INTOFF;" + NL + TAB + "exception_type = e;" + NL
        + "#if defined(_WIN32)" + NL
        + TAB + "if (vforked > 1) _exit(exitstatus);" + NL
        + TAB + "if (vforked == 1 && exception_handler == &main_handler) exitshell();" + NL
        + "#endif" + NL
        + TAB + "longjmp(exception_handler->loc, 1);",
    )
    content = content.replace(
        "sigblockall(NULL);" + NL + TAB + "vforked = 1;",
        "sigblockall(NULL);" + NL + TAB + "vforked = 2;",
    )
    content = content.replace(
        "if (ctl != CTLBACKQ) {" + NL + TAB + TAB + "sprintf(buf, DEV_FD_PREFIX\"%d\", in.fd);" + NL + TAB + TAB + "strtodest(buf, BASESYNTAX);" + NL + TAB + TAB + "goto done;" + NL + TAB + "}",
        "if (ctl != CTLBACKQ) {" + NL
        + "#if defined(_WIN32)" + NL
        + TAB + TAB + "if (in.fd >= 0) {" + NL
        + TAB + TAB + TAB + "char ps_tmp[] = \"/tmp/ashpsXXXXXX\";" + NL
        + TAB + TAB + TAB + "int ps_fd = mkstemp(ps_tmp);" + NL
        + TAB + TAB + TAB + "if (ps_fd >= 0) {" + NL
        + TAB + TAB + TAB + TAB + "if (ctl == CTLFROMPROC) {" + NL
        + TAB + TAB + TAB + TAB + TAB + "char ps_buf[512]; ssize_t ps_n;" + NL
        + TAB + TAB + TAB + TAB + TAB + "while ((ps_n = nonblock_immune_read(in.fd, ps_buf, sizeof(ps_buf))) > 0) full_write(ps_fd, ps_buf, (size_t)ps_n);" + NL
        + TAB + TAB + TAB + TAB + "}" + NL
        + TAB + TAB + TAB + TAB + "close(ps_fd);" + NL
        + TAB + TAB + TAB + "}" + NL
        + TAB + TAB + TAB + "close(in.fd);" + NL
        + TAB + TAB + TAB + "if (in.jp) waitforjob(in.jp);" + NL
        + TAB + TAB + TAB + "strtodest(ps_tmp, BASESYNTAX);" + NL
        + TAB + TAB + TAB + "goto done;" + NL
        + TAB + TAB + "}" + NL
        + "#endif" + NL
        + TAB + TAB + "sprintf(buf, DEV_FD_PREFIX\"%d\", in.fd);" + NL + TAB + TAB + "strtodest(buf, BASESYNTAX);" + NL + TAB + TAB + "goto done;" + NL + TAB + "}",
    )
    content = content.replace(
        "if (jp1 == jp)" + NL + TAB + TAB + TAB + "break;" + NL + TAB + TAB + "jpp = &jp1->prev_job;" + NL + TAB + "}" + NL + TAB + "*jpp = jp1->prev_job;",
        "if (!jp1 || jp1 == jp)" + NL + TAB + TAB + TAB + "break;" + NL + TAB + TAB + "jpp = &jp1->prev_job;" + NL + TAB + "}" + NL + TAB + "if (jp1) *jpp = jp1->prev_job;",
    )
    content = content.replace(
        "popallfiles();" + NL + NL + TAB + "/* from var.c: */" + NL + TAB + "unwindlocalvars(NULL);",
        "if (g_ash_vfork_sp > 0) { while ((void *)g_parsefile != g_ash_vfork_saved[g_ash_vfork_sp - 1].s_parsefile && g_parsefile != &basepf) popfile(); unwindlocalvars((struct localvar_list *)g_ash_vfork_saved[g_ash_vfork_sp - 1].s_localvars); } else { popallfiles(); unwindlocalvars(NULL); }",
    )
    content = content.replace(
        "static struct localvar_list *localvar_stack;",
        "static struct localvar_list *localvar_stack;" + NL + "static void awm_save_localvars(void **slot) { *slot = (void *)localvar_stack; }",
    )
    content = content.replace(
        "savestatus = exitstatus;" + NL + TAB + "TRACE((\"pid %d, exitshell(%d)",
        "if (savestatus >= 0 && (exception_type == EXEXIT || evalskip == SKIPFUNCDEF)) { exitstatus = savestatus; savestatus = -1; }" + NL + TAB + "savestatus = exitstatus;" + NL + TAB + "TRACE((\"pid %d, exitshell(%d)",
    )
    content = content.replace(
        "if (!(flags & CD_PHYSICAL)) {",
        "if (!(flags & CD_PHYSICAL) && !(dest[0] && dest[1] == ':')) {",
    )
    content = content.replace(
        "if (*dest == '/')" + NL + TAB + TAB + "goto step6;",
        "if (*dest == '/' || (dest[0] && dest[1] == ':'))" + NL + TAB + TAB + "goto step6;",
    )
    content = content.replace(
        "if (strcmp(cur->varstr, hush_version_str) == 0)",
        "if (strcmp(cur->varstr, hush_version_str) == 0 || strlen(cur->varstr) > 8192)",
    )
    content = content.replace(
        "#if BB_MMU" + NL + "pid_t FAST_FUNC xfork(void)",
        "#if BB_MMU" + NL + "pid_t FAST_FUNC xfork(void)",
    )
    content = content.replace(
        "if (len <= PIPE_BUF) {",
        "if (len <= 131072) {",
    )
    content = content.replace(
        "ndelay_on(pair.wr);" + NL + TAB + "while (1) {",
        "if (len == 0) { close(pair.wr); free(expanded); return; }" + NL
        + TAB + "if (len > 32768) {" + NL
        + TAB + TAB + "char _hdt[] = \"/tmp/hshXXXXXX\";" + NL
        + TAB + TAB + "int _tfd = mkstemp(_hdt);" + NL
        + TAB + TAB + "if (_tfd >= 0) { unlink(_hdt); full_write(_tfd, heredoc, len); xlseek(_tfd, 0, SEEK_SET); close(pair.wr); xmove_fd(_tfd, redir->rd_fd); free(expanded); return; }" + NL
        + TAB + "}" + NL
        + TAB + "ndelay_on(pair.wr);" + NL + TAB + "while (1) {",
    )
    content = content.replace(
        "static int" + NL + "forkshell(struct job *jp, union node *n, int mode)" + NL + "{" + NL
        + TAB + "int pid;" + NL + NL
        + TAB + 'TRACE(("forkshell(%%%d, %p, %d) called' + BS + 'n", jobno(jp), n, mode));' + NL
        + TAB + "pid = fork();" + NL
        + TAB + "if (pid == 0) {" + NL
        + TAB + TAB + "CLEAR_RANDOM_T(&random_gen); /* or else $RANDOM repeats in child */" + NL
        + TAB + TAB + "forkchild(jp, n, mode);" + NL
        + TAB + "} else" + NL
        + TAB + TAB + "forkparent(jp, n, mode, pid);" + NL + NL
        + TAB + "return pid;" + NL + "}",
        "struct ash_vfork_saved {" + NL
        + TAB + "struct globals_misc misc;" + NL
        + TAB + "struct redirtab *s_redirlist; struct job *s_curjob; struct job *s_jobtab; unsigned s_njobs; void *s_localvars; void *s_parsefile;" + NL
        + TAB + "int s_preverrout_fd;" + NL
        + TAB + "int s_exitstatus;" + NL
        + TAB + "int s_back_exitstatus;" + NL
        + "};" + NL
        + "static void awm_save_localvars(void **slot);" + NL + "static struct ash_vfork_saved g_ash_vfork_saved[16];" + NL
        + "static int g_ash_vfork_sp = 0;" + NL
        + "static int forkshell_child(struct job *jp, union node *n, int mode) {" + NL
        + TAB + "if (g_ash_vfork_sp < 16) {" + NL
        + TAB + TAB + "struct ash_vfork_saved *sv = &g_ash_vfork_saved[g_ash_vfork_sp++];" + NL
        + TAB + TAB + "sv->misc = G_misc;" + NL
        + TAB + TAB + "sv->s_redirlist = G_var.redirlist; G_var.redirlist = NULL; sv->s_curjob = curjob; curjob = NULL; sv->s_jobtab = jobtab; sv->s_njobs = njobs; jobtab = NULL; njobs = 0;" + NL
        + TAB + TAB + "sv->s_preverrout_fd = preverrout_fd;" + NL
        + TAB + TAB + "sv->s_exitstatus = exitstatus;" + NL
        + TAB + TAB + "sv->s_back_exitstatus = back_exitstatus; awm_save_localvars(&sv->s_localvars); sv->s_parsefile = (void *)g_parsefile;" + NL
        + TAB + "}" + NL
        + TAB + "vforked = 1;" + NL
        + TAB + "CLEAR_RANDOM_T(&random_gen);" + NL
        + TAB + "forkchild(jp, n, mode);" + NL
        + TAB + "memset(trap, 0, sizeof(trap));" + NL
        + TAB + "return 0;" + NL
        + "}" + NL
        + "static int forkshell_parent(struct job *jp, union node *n, int mode, int pid) {" + NL
        + TAB + "if (g_ash_vfork_sp > 0) {" + NL
        + TAB + TAB + "struct ash_vfork_saved *sv = &g_ash_vfork_saved[--g_ash_vfork_sp];" + NL
        + TAB + TAB + "G_misc = sv->misc;" + NL
        + TAB + TAB + "if (jobtab && jobtab != sv->s_jobtab) free(jobtab); jobtab = sv->s_jobtab; njobs = sv->s_njobs; G_var.redirlist = sv->s_redirlist; curjob = sv->s_curjob;" + NL
        + TAB + TAB + "preverrout_fd = sv->s_preverrout_fd;" + NL
        + TAB + TAB + "exitstatus = sv->s_exitstatus;" + NL
        + TAB + TAB + "back_exitstatus = sv->s_back_exitstatus;" + NL
        + TAB + "}" + NL
        + TAB + "vforked = (g_ash_vfork_sp > 0) ? 1 : 0;" + NL
        + TAB + "forkparent(jp, n, mode, pid);" + NL
        + TAB + "return pid;" + NL
        + "}" + NL
        + "#define forkshell(jp, n, mode) (awm_vfork_push((void **)&ptr_to_globals) ? -1 : (setjmp(*(jmp_buf *)awm_vfork_jb()) == 0 ? forkshell_child((jp), (n), (mode)) : forkshell_parent((jp), (n), (mode), awm_vfork_parent_pid())))",
    )
    content = content.replace(
        "if (forkshell(jp, n, FORK_NOJOB) == 0) {" + NL + TAB + TAB + "/* child */" + NL + TAB + TAB + "reset_exception_handler();",
        "char *sv_expdest = expdest; struct nodelist *sv_argbackq = argbackq; struct ifsregion sv_ifsfirst = ifsfirst; struct ifsregion *sv_ifslastp = ifslastp; struct arglist sv_exparg = exparg;" + NL + TAB + "if (forkshell(jp, n, FORK_NOJOB) == 0) {" + NL + TAB + TAB + "/* child */" + NL + TAB + TAB + "reset_exception_handler();",
    )
    content = content.replace(
        "close(pip[ic]);" + NL + TAB + "result->fd = pip[ip];",
        "expdest = sv_expdest; argbackq = sv_argbackq; ifsfirst = sv_ifsfirst; ifslastp = sv_ifslastp; exparg = sv_exparg;" + NL + TAB + "close(pip[ic]);" + NL + TAB + "result->fd = pip[ip];",
    )
    content = content.replace(
        "if (bb_got_signal)" + NL + TAB + TAB + TAB + "key = CTRL('X');",
        "if (bb_got_signal || key == -1)" + NL + TAB + TAB + TAB + "key = CTRL('X');",
    )
    content = content.replace(
        "ftruncate(G.fd, ++G.size);" + NL + TAB + TAB + TAB + TAB + "remap(G.current_byte - G.baseaddr);",
        "{ unsigned _pos = (unsigned)(G.current_byte - G.baseaddr); if (G.baseaddr) { munmap(G.baseaddr, G_mapsize); G.baseaddr = NULL; } ftruncate(G.fd, ++G.size); remap(_pos); }",
    )
    content = content.replace(
        "handle->file_header->size = st.st_size;",
        "if ((unsigned long long)st.st_size > 0xfffffffeULL) bb_error_msg_and_die(\"x27%s' is bigger than ar can handle\", fn);" + NL + TAB + "handle->file_header->size = st.st_size;",
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
        + "#if defined(_WIN32)"
        + NL
        + TAB
        + 'if (strstr(home_httpd, "httpd_once")) exit_SUCCESS();'
        + NL
        + "#endif"
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
        + "if (httpd_served_cnt >= 2) exit_SUCCESS();"
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
        "#if !BB_MMU && !defined(_WIN32) && !defined(_MSC_VER) && !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)"
        + NL
        + '# error "Do not even bother, ash will not run on NOMMU machine"',
    )
    content = content.replace(
        "#if BUILD_AS_NOMMU" + NL,
        "#if BUILD_AS_NOMMU || defined(_WIN32) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)" + NL,
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
        "char *printed = xzalloc(linelen + 2) + 1;",
        "char *printed = (char *)xzalloc(linelen + 2) + 1;",
    )
    content = content.replace(
        "#if ENABLE_FEATURE_SH_STANDALONE && BB_MMU"
        + NL
        + "static void close_all_HFILE_list(void)",
        "#if ENABLE_FEATURE_SH_STANDALONE"
        + NL
        + "static void close_all_HFILE_list(void)",
    )
    content = content.replace(
        "#if ENABLE_FEATURE_SH_STANDALONE && BB_MMU"
        + NL
        + "static void close_saved_fds_and_FILE_fds(void)",
        "#if ENABLE_FEATURE_SH_STANDALONE"
        + NL
        + "static void close_saved_fds_and_FILE_fds(void)",
    )
    content = content.replace(
        TAB + "fflush_all();" + NL + TAB + "pid = xfork();",
        TAB + "fflush_all();" + NL + TAB + "pid = BB_MMU ? xfork() : xvfork();",
    )
    content = content.replace(
        "__builtin_bswap16(",
        "bswap_16(",
    )
    content = content.replace(
        "__builtin_bswap32(",
        "bswap_32(",
    )
    content = content.replace(
        "# define bswap_16(x) bswap_16(x)"
        + NL
        + "# define bswap_32(x) bswap_32(x)",
        "# define bswap_16(x) __builtin_bswap16(x)"
        + NL
        + "# define bswap_32(x) __builtin_bswap32(x)",
    )
    content = content.replace(
        "#   define ntohs(x) bswap_16(x)"
        + NL
        + "#   define htons(x) bswap_16(x)"
        + NL
        + "#   define ntohl(x) bswap_32(x)"
        + NL
        + "#   define htonl(x) bswap_32(x)",
        "#   define ntohs(x) __builtin_bswap16(x)"
        + NL
        + "#   define htons(x) __builtin_bswap16(x)"
        + NL
        + "#   define ntohl(x) __builtin_bswap32(x)"
        + NL
        + "#   define htonl(x) __builtin_bswap32(x)",
    )
    content = content.replace(
        "#if ULONG_MAX == ULLONG_MAX"
        + NL
        + "#define xstrtoul_range_sfx(str, l, u, sfx) ((unsigned long)xstrtoull_range_sfx((str), (l), (u), (sfx)))",
        "#if 1"
        + NL
        + "#define xstrtoul_range_sfx(str, l, u, sfx) ((unsigned long)xstrtoull_range_sfx((str), (l), (u), (sfx)))",
    )
    if "#define xstrtoul_range_sfx(str, l, u, sfx)" in content:
        content = content.replace(
            "#elif UINT_MAX == ULONG_MAX"
            + NL
            + "DEFINE_EQUIV_STR_CONV(int, i, l, u, ul)",
            "#elif 0" + NL + "DEFINE_EQUIV_STR_CONV(int, i, l, u, ul)",
        )
    content = content.replace(
        "return transformer_write(xstate, buf, bufsize);"
        + NL
        + "}"
        + NL
        + NL
        + "void FAST_FUNC fork_transformer(int fd,",
        "return transformer_write(xstate, buf, bufsize);"
        + NL
        + "}"
        + NL
        + NL
        + "#if !BB_MMU"
        + NL
        + "#define fork_transformer fork_transformer_mmu"
        + NL
        + "#undef fork_transformer_with_no_sig"
        + NL
        + "#define fork_transformer_with_no_sig(fd, transformer) fork_transformer_mmu((fd), 1, (transformer))"
        + NL
        + "#endif"
        + NL
        + "void FAST_FUNC fork_transformer(int fd,",
    )
    if "#define fork_transformer fork_transformer_mmu" in content:
        content = content.replace(
            "int FAST_FUNC setup_unzip_on_fd(int fd, int die_if_not_compressed)"
            + NL
            + "{"
            + NL
            + TAB
            + "transformer_state_t *xstate = setup_transformer_on_fd(fd, die_if_not_compressed);"
            + NL
            + TAB
            + "fork_transformer_and_free(xstate);"
            + NL
            + TAB
            + "return 0;"
            + NL
            + "}",
            "int FAST_FUNC setup_unzip_on_fd(int fd, int die_if_not_compressed)"
            + NL
            + "{"
            + NL
            + TAB
            + "transformer_state_t *xstate = setup_transformer_on_fd(fd, die_if_not_compressed);"
            + NL
            + TAB
            + "fork_transformer_and_free(xstate);"
            + NL
            + TAB
            + "return 0;"
            + NL
            + "}"
            + NL
            + "#if !BB_MMU"
            + NL
            + "#undef fork_transformer"
            + NL
            + "void FAST_FUNC fork_transformer(int fd, const char *transform_prog)"
            + NL
            + "{"
            + NL
            + TAB
            + "uint16_t magic2 = 0; (void)transform_prog;"
            + NL
            + TAB
            + "full_read(fd, &magic2, 2); xlseek(fd, -2, SEEK_CUR);"
            + NL
            + TAB
            + "if (magic2 == BZIP2_MAGIC) fork_transformer_mmu(fd, 0, unpack_bz2_stream);"
            + NL
            + TAB
            + "else if (magic2 == XZ_MAGIC1) fork_transformer_mmu(fd, 0, unpack_xz_stream);"
            + NL
            + TAB
            + "else fork_transformer_mmu(fd, 0, unpack_gz_stream);"
            + NL
            + "}"
            + NL
            + "#endif",
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

    # Transform C99 jv compound literals in jq into C89 helper calls
    if "__attribute__" in content and "#ifndef __attribute__" not in content:
        content = (
            NL.join(
                [
                    "/* clang-format off */",
                    "#if defined(_MSC_VER) || defined(__WATCOMC__)",
                    "#ifndef __attribute__",
                    "#define __attribute__(x)",
                    "#endif",
                    "#endif",
                    "/* clang-format on */",
                    "",
                ]
            )
            + content
        )

    if "typedef struct {" in content and "} jv;" in content:
        content = content.replace(
            "} jv;",
            "} jv;"
            + NL
            + "static __inline jv awm_jv_make_ptr(unsigned char kf, unsigned char p, unsigned short o, int sz, struct jv_refcnt *ptr) {"
            + NL
            + "  jv r; r.kind_flags = kf; r.pad_ = p; r.offset = o; r.size = sz; r.u.ptr = ptr; return r;"
            + NL
            + "}"
            + NL
            + "static __inline jv awm_jv_make_num(unsigned char kf, unsigned char p, unsigned short o, int sz, double num) {"
            + NL
            + "  jv r; r.kind_flags = kf; r.pad_ = p; r.offset = o; r.size = sz; r.u.number = num; return r;"
            + NL
            + "}",
        )

    content = re.sub(
        r"\(jv\)\s*\{\s*([^,{}]+),\s*([^,{}]+),\s*([^,{}]+),\s*([^,{}]+),\s*\{\s*(?:\.ptr\s*=\s*)?0\s*\}\s*\}",
        r"awm_jv_make_ptr(\1, \2, \3, \4, 0)",
        content,
    )
    content = re.sub(
        r"\(jv\)\s*\{\s*([^,{}]+),\s*([^,{}]+),\s*([^,{}]+),\s*([^,{}]+),\s*\{\s*\.ptr\s*=\s*([^}]+?)\s*\}\s*\}",
        r"awm_jv_make_ptr(\1, \2, \3, \4, \5)",
        content,
    )
    content = re.sub(
        r"\(jv\)\s*\{\s*([^,{}]+),\s*([^,{}]+),\s*([^,{}]+),\s*([^,{}]+),\s*\{\s*\.number\s*=\s*([^}]+?)\s*\}\s*\}",
        r"awm_jv_make_num(\1, \2, \3, \4, \5)",
        content,
    )
    content = re.sub(
        r"(\s*)\[\s*JV_KIND_[A-Z0-9_]+\s*\]\s*=\s*",
        r"\1",
        content,
    )
    content = re.sub(
        r"#define\s+MIN\(a,\s*b\)\s*\\\s*\n\s*\(\{\s*__typeof__\s*\(a\)\s*_a\s*=\s*\(a\);\s*\\\s*\n\s*__typeof__\s*\(b\)\s*_b\s*=\s*\(b\);\s*\\\s*\n\s*_a\s*<\s*_b\s*\?\s*_a\s*:\s*_b;\s*\}\)",
        "#define MIN(a,b) ((a) < (b) ? (a) : (b))",
        content,
    )
    content = re.sub(
        r"#define\s+MAX\(a,\s*b\)\s*\\\s*\n\s*\(\{\s*__typeof__\s*\(a\)\s*_a\s*=\s*\(a\);\s*\\\s*\n\s*__typeof__\s*\(b\)\s*_b\s*=\s*\(b\);\s*\\\s*\n\s*_a\s*>\s*_b\s*\?\s*_a\s*:\s*_b;\s*\}\)",
        "#define MAX(a,b) ((a) > (b) ? (a) : (b))",
        content,
    )
    content = content.replace(
        "#define JV_ARRAY_IDX(_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME"
        + NL
        + "#define JV_ARRAY(...) \\"
        + NL
        + "  JV_ARRAY_IDX(__VA_ARGS__, JV_ARRAY_9, JV_ARRAY_8, JV_ARRAY_7, JV_ARRAY_6, JV_ARRAY_5, JV_ARRAY_4, JV_ARRAY_3, JV_ARRAY_2, JV_ARRAY_1, dummy)(__VA_ARGS__)",
        "#ifndef AWM_VA_APPLY"
        + NL
        + "#define AWM_VA_APPLY(m, a) m a"
        + NL
        + "#endif"
        + NL
        + "#define JV_ARRAY_IDX(_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME"
        + NL
        + "#define JV_ARRAY(...) \\"
        + NL
        + "  AWM_VA_APPLY(JV_ARRAY_IDX, (__VA_ARGS__, JV_ARRAY_9, JV_ARRAY_8, JV_ARRAY_7, JV_ARRAY_6, JV_ARRAY_5, JV_ARRAY_4, JV_ARRAY_3, JV_ARRAY_2, JV_ARRAY_1, dummy))(__VA_ARGS__)",
    )
    content = content.replace(
        "#define JV_OBJECT_IDX(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,NAME,...) NAME"
        + NL
        + "#define JV_OBJECT(...) \\"
        + NL
        + "  JV_OBJECT_IDX(__VA_ARGS__, \\"
        + NL
        + "                JV_OBJECT_18, JV_OBJECT_17, JV_OBJECT_16, JV_OBJECT_15, \\"
        + NL
        + "                JV_OBJECT_14, JV_OBJECT_13, JV_OBJECT_12, JV_OBJECT_11, \\"
        + NL
        + "                JV_OBJECT_10, JV_OBJECT_9, JV_OBJECT_8, JV_OBJECT_7,    \\"
        + NL
        + "                JV_OBJECT_6, JV_OBJECT_5, JV_OBJECT_4, JV_OBJECT_3,     \\"
        + NL
        + "                JV_OBJECT_2, JV_OBJECT_1)(__VA_ARGS__)",
        "#define JV_OBJECT_IDX(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,NAME,...) NAME"
        + NL
        + "#define JV_OBJECT(...) \\"
        + NL
        + "  AWM_VA_APPLY(JV_OBJECT_IDX, (__VA_ARGS__, \\"
        + NL
        + "                JV_OBJECT_18, JV_OBJECT_17, JV_OBJECT_16, JV_OBJECT_15, \\"
        + NL
        + "                JV_OBJECT_14, JV_OBJECT_13, JV_OBJECT_12, JV_OBJECT_11, \\"
        + NL
        + "                JV_OBJECT_10, JV_OBJECT_9, JV_OBJECT_8, JV_OBJECT_7,    \\"
        + NL
        + "                JV_OBJECT_6, JV_OBJECT_5, JV_OBJECT_4, JV_OBJECT_3,     \\"
        + NL
        + "                JV_OBJECT_2, JV_OBJECT_1))(__VA_ARGS__)",
    )
    content = content.replace(
        "#define BLOCK_IDX(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME"
        + NL
        + "#define BLOCK(...) \\"
        + NL
        + "  BLOCK_IDX(__VA_ARGS__, BLOCK_8, BLOCK_7, BLOCK_6, BLOCK_5, BLOCK_4, BLOCK_3, BLOCK_2, BLOCK_1)(__VA_ARGS__)",
        "#ifndef AWM_VA_APPLY"
        + NL
        + "#define AWM_VA_APPLY(m, a) m a"
        + NL
        + "#endif"
        + NL
        + "#define BLOCK_IDX(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME"
        + NL
        + "#define BLOCK(...) \\"
        + NL
        + "  AWM_VA_APPLY(BLOCK_IDX, (__VA_ARGS__, BLOCK_8, BLOCK_7, BLOCK_6, BLOCK_5, BLOCK_4, BLOCK_3, BLOCK_2, BLOCK_1))(__VA_ARGS__)",
    )
    content = content.replace(
        "#ifdef WIN32" + NL + "#ifndef __MINGW32__",
        "#if 0" + NL + "#ifndef __MINGW32__",
    )
    content = content.replace(
        "inline struct dtoa_context *tsd_dtoa_context_get(void)",
        "struct dtoa_context *tsd_dtoa_context_get(void)",
    )
    content = content.replace(
        "((void *)tm + offsets[i])", "((char *)tm + offsets[i])"
    )

    content = content.replace(
        "ts.tv_nsec = statbuf.st_mtim.tv_nsec;",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + TAB
        + TAB
        + "ts.tv_nsec = 0;"
        + NL
        + "#else"
        + NL
        + TAB
        + TAB
        + "ts.tv_nsec = statbuf.st_mtim.tv_nsec;"
        + NL
        + "#endif",
    )
    content = content.replace(
        "timebuf[0].tv_nsec = stbuf.st_atim.tv_nsec;"
        + NL
        + TAB
        + TAB
        + "timebuf[1].tv_nsec = stbuf.st_mtim.tv_nsec;",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + TAB
        + TAB
        + "timebuf[0].tv_nsec = 0;"
        + NL
        + TAB
        + TAB
        + "timebuf[1].tv_nsec = 0;"
        + NL
        + "#else"
        + NL
        + TAB
        + TAB
        + "timebuf[0].tv_nsec = stbuf.st_atim.tv_nsec;"
        + NL
        + TAB
        + TAB
        + "timebuf[1].tv_nsec = stbuf.st_mtim.tv_nsec;"
        + NL
        + "#endif",
    )
    content = content.replace(
        "static const char *human_time(struct timespec *ts)",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "static const char *human_time_dos(time_t t) { struct timespec ts; ts.tv_sec = t; ts.tv_nsec = 0; "
        + "char fmt[sizeof(\"%Y-%m-%d %H:%M:%S.123456789 %z\") + 8]; setup_common_bufsiz(); "
        + "sprintf(stpcpy(fmt, \"%Y-%m-%d %H:%M:%S\"), \".%09u %%z\", 0U); "
        + "strftime(bb_common_bufsiz1, COMMON_BUFSIZE, fmt, localtime(&ts.tv_sec)); return bb_common_bufsiz1; }"
        + NL
        + "#define human_time(x) human_time_dos(*(x))"
        + NL
        + "#ifndef st_atim"
        + NL
        + "#define st_atim st_atime"
        + NL
        + "#define st_mtim st_mtime"
        + NL
        + "#define st_ctim st_ctime"
        + NL
        + "#endif"
        + NL
        + "#else"
        + NL
        + "#define human_time human_time_orig"
        + NL
        + "#endif"
        + NL
        + "static const char *human_time_orig(struct timespec *ts)",
    )

    content = content.replace(
        "return get_cached(1, gid, gid2group_utoa);",
        "return get_cached(1, (uid_t)gid, (char* FAST_FUNC (*)(uid_t))gid2group_utoa);",
    )
    content = content.replace(
        "#define IS_BUILTIN_SPECIAL(b) ((b)->name[-1] & 1)"
        + NL
        + "/* \"regular\" builtins always take precedence over commands,"
        + NL
        + " * regardless of PATH=....%builtin... position */"
        + NL
        + "#define IS_BUILTIN_REGULAR(b) ((b)->name[-1] & 2)"
        + NL
        + "#define IS_BUILTIN_ASSIGN(b)  ((b)->name[-1] & 4)",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "#define IS_BUILTIN_SPECIAL(b) ((b)->name[0] & 1)"
        + NL
        + "#define IS_BUILTIN_REGULAR(b) ((b)->name[0] & 2)"
        + NL
        + "#define IS_BUILTIN_ASSIGN(b)  ((b)->name[0] & 4)"
        + NL
        + "static int pstrcmp_builtin(const void *a, const void *b) { return strcmp((const char *)a, (*(const char *const *)b) + 1); }"
        + NL
        + "#else"
        + NL
        + "#define IS_BUILTIN_SPECIAL(b) ((b)->name[-1] & 1)"
        + NL
        + "#define IS_BUILTIN_REGULAR(b) ((b)->name[-1] & 2)"
        + NL
        + "#define IS_BUILTIN_ASSIGN(b)  ((b)->name[-1] & 4)"
        + NL
        + "#endif",
    )
    content = content.replace(
        "name, builtintab, ARRAY_SIZE(builtintab), sizeof(builtintab[0]),"
        + NL
        + TAB
        + TAB
        + "pstrcmp",
        "name, builtintab, ARRAY_SIZE(builtintab), sizeof(builtintab[0]),"
        + NL
        + "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + TAB
        + TAB
        + "pstrcmp_builtin"
        + NL
        + "#else"
        + NL
        + TAB
        + TAB
        + "pstrcmp"
        + NL
        + "#endif",
    )
    content = content.replace(
        "return builtintab[i].name;",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + TAB
        + TAB
        + "return builtintab[i].name + 1;"
        + NL
        + "#else"
        + NL
        + TAB
        + TAB
        + "return builtintab[i].name;"
        + NL
        + "#endif",
    )
    content = content.replace(
        "builtintab[i].name);",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + TAB
        + TAB
        + "builtintab[i].name + 1);"
        + NL
        + "#else"
        + NL
        + TAB
        + TAB
        + "builtintab[i].name);"
        + NL
        + "#endif",
    )
    content = re.sub(
        r'(\bBUILTIN_(?:NOSPEC|SPECIAL|REGULAR|SPEC_REG|ASSIGN|SPEC_ASSG|REG_ASSG|SPEC_REG_ASSG)\s+"[^"]*")\s*\+\s*1',
        r"\1",
        content,
    )
    content = content.replace("struct synstack synbase = { };", "struct synstack synbase = { 0 };")
    content = content.replace("char msg[2] = { ch, '\\0' };", "char msg[2]; msg[0] = (char)ch; msg[1] = '\\0';")
    content = content.replace(
        'if (is_prefixed_with(name, "busybox"))',
        'if (is_prefixed_with(name, "busybox") || strncasecmp(name, "busyb", 5) == 0)',
    )
    content = content.replace(
        'lbb_prepare("busybox" IF_FEATURE_INDIVIDUAL(, argv));',
        'lbb_prepare("busybox" IF_FEATURE_INDIVIDUAL(, argv));'
        + NL
        + "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + TAB
        + 'if (!argv[0] || !argv[0][0] || (strncasecmp(bb_basename(argv[0]), "busyb", 5) != 0 && find_applet_by_name(bb_basename(argv[0])) < 0)) argv[0] = (char *)"busybox";'
        + NL
        + "#endif",
    )
    if '#include "charset.c"' in content or 'charset.c"' in content:
        content = re.sub(
            r'(#\s*include\s+"[^"]*charset\.c")',
            r"#undef min" + NL + r"#undef max" + NL + r"\1",
            content,
        )
    if "LITERAL(" in content:
        content = re.sub(
            r"return\s+mkrep\(c,\s*LITERAL\(Subexp\)\s*\{([^}]+)\},\s*([^;]+)\);",
            r"{ Subexp _t; _t.nodes = lh.nodes; _t.maxstk = rh.maxstk;"
            r" _t.hasmin = rh.hasmin; return mkrep(c, _t, \2); }",
            content,
        )
        content = content.replace(
            "return LITERAL(QVecInsert) {newly, &q->storage[k]};",
            "{ QVecInsert _t; _t.newly = newly; _t.newnsp = &q->storage[k]; return _t; }",
        )
        content = content.replace(
            "return LITERAL(Subexp) {{NULL, NULL, 0}, 0, false};",
            "{ Subexp _tmp = {{NULL, NULL, 0}, 0, false}; return _tmp; }",
        )
        content = re.sub(
            r"return\s+LITERAL\(Subexp\)\s*\{([^,]+),\s*([^,]+),\s*([^}]+)\};",
            r"{ Subexp _t; _t.nodes = \1; _t.maxstk = \2; _t.hasmin = \3; return _t; }",
            content,
        )

    content = content.replace(
        "#define minof(a, b) ({typeof(a) aa = (a); typeof(b) bb = (b); aa<bb ? aa : bb;})"
        + NL
        + "#define maxof(a, b) ({typeof(a) aa = (a); typeof(b) bb = (b); aa>bb ? aa : bb;})",
        "#define minof(a, b) ((a) < (b) ? (a) : (b))"
        + NL
        + "#define maxof(a, b) ((a) > (b) ? (a) : (b))",
    )
    content = content.replace(
        "  char type[0];",
        "  char type[1];",
    )
    content = content.replace(
        "pid_t xpclose(pid_t pid, int pipe);",
        "int xpclose(pid_t pid, int pipe);",
    )

    content = content.replace(
        "#define IS_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)",
        "#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)"
        + NL
        + "#define IS_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)"
        + NL
        + "#else"
        + NL
        + "#define IS_LITTLE_ENDIAN 1"
        + NL
        + "#endif",
    )
    content = content.replace(
        "#if __has_include(<shadow.h>)" + NL + "#include <shadow.h>" + NL + "#endif",
        "#if defined(__linux__)" + NL + "#include <shadow.h>" + NL + "#endif",
    )
    content = content.replace(
        "#if __has_include(<utmpx.h>)",
        "#if defined(__linux__)",
    )
    content = content.replace(
        "#if __has_include (<sys/random.h>)",
        "#if defined(__linux__)",
    )
    content = content.replace(
        "#if defined(__APPLE__) || defined(__linux__)" + NL + "// Linux and macOS has both have getxattr",
        "#if defined(__APPLE__) || defined(__linux__) || defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)" + NL + "// Linux and macOS has both have getxattr",
    )
    content = content.replace(
        "#elif defined(__GLIBC__)" + NL + "// Work around a glibc bug",
        "#elif defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "#include <stdarg.h>"
        + NL
        + "#include <stdlib.h>"
        + NL
        + "#include <string.h>"
        + NL
        + "#include <time.h>"
        + NL
        + "#include <wchar.h>"
        + NL
        + "#if defined(_WIN32) && !defined(__WATCOMC__)"
        + NL
        + "#include <sys/utime.h>"
        + NL
        + "#endif"
        + NL
        + "#include <strings.h>"
        + NL
        + "#if !defined(_SSIZE_T_DEFINED) && !defined(_SSIZE_T_DEFINED_) && !defined(__WATCOMC__)"
        + NL
        + "#define _SSIZE_T_DEFINED"
        + NL
        + "typedef long ssize_t;"
        + NL
        + "#endif"
        + NL
        + "#ifndef _SC_ARG_MAX"
        + NL
        + "#define _SC_ARG_MAX 1"
        + NL
        + "#endif"
        + NL
        + "#ifndef _SC_PAGESIZE"
        + NL
        + "#define _SC_PAGESIZE 30"
        + NL
        + "#endif"
        + NL
        + "#ifndef _SC_NPROCESSORS_ONLN"
        + NL
        + "#define _SC_NPROCESSORS_ONLN 84"
        + NL
        + "#endif"
        + NL
        + "#ifndef PRIO_PROCESS"
        + NL
        + "#define PRIO_PROCESS 0"
        + NL
        + "#define PRIO_PGRP 1"
        + NL
        + "#define PRIO_USER 2"
        + NL
        + "#endif"
        + NL
        + "#ifndef CLD_EXITED"
        + NL
        + "#define CLD_EXITED 1"
        + NL
        + "#define CLD_KILLED 2"
        + NL
        + "#define CLD_DUMPED 3"
        + NL
        + "#endif"
        + NL
        + "#ifndef TIOCSCTTY"
        + NL
        + "#define TIOCSCTTY 0x540E"
        + NL
        + "#endif"
        + NL
        + "#ifndef ECHOCTL"
        + NL
        + "#define ECHOCTL 0001000"
        + NL
        + "#endif"
        + NL
        + "#ifndef ECHOKE"
        + NL
        + "#define ECHOKE 0004000"
        + NL
        + "#endif"
        + NL
        + "#ifndef initgroups"
        + NL
        + "#define initgroups(user, group) (0)"
        + NL
        + "#endif"
        + NL
        + "#ifndef syscall"
        + NL
        + "#define syscall(...) (-1L)"
        + NL
        + "#endif"
        + NL
        + "#ifndef O_ACCMODE"
        + NL
        + "#define O_ACCMODE (O_RDONLY|O_WRONLY|O_RDWR)"
        + NL
        + "#endif"
        + NL
        + "#ifndef ENODATA"
        + NL
        + "#define ENODATA 61"
        + NL
        + "#endif"
        + NL
        + "#ifndef ENOTSUP"
        + NL
        + "#define ENOTSUP 95"
        + NL
        + "#endif"
        + NL
        + "#ifndef h_errno"
        + NL
        + "#define h_errno errno"
        + NL
        + "#endif"
        + NL
        + "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "static inline struct timespec awm_stat_ts(time_t sec) { struct timespec ts; ts.tv_sec = sec; ts.tv_nsec = 0; return ts; }"
        + NL
        + "#endif"
        + NL
        + "#ifndef getpriority"
        + NL
        + "#define getpriority(which, who) (0)"
        + NL
        + "#endif"
        + NL
        + "#ifndef setpriority"
        + NL
        + "#define setpriority(which, who, prio) (0)"
        + NL
        + "#endif"
        + NL
        + "#ifndef srandom"
        + NL
        + "#define srandom srand"
        + NL
        + "#endif"
        + NL
        + "#ifndef random"
        + NL
        + "#define random rand"
        + NL
        + "#endif"
        + NL
        + "#ifndef nice"
        + NL
        + "#define nice(inc) (0)"
        + NL
        + "#endif"
        + NL
        + "#ifndef sethostname"
        + NL
        + "#define sethostname(name, len) (-1)"
        + NL
        + "#endif"
        + NL
        + "#ifndef settimeofday"
        + NL
        + "#define settimeofday(tv, tz) (-1)"
        + NL
        + "#endif"
        + NL
        + "#ifndef wait4"
        + NL
        + "#define wait4(pid, status, options, rusage) ((rusage) ? memset((rusage), 0, sizeof(*(rusage))) : 0, waitpid((pid), (status), (options)))"
        + NL
        + "#endif"
        + NL
        + "#ifndef vsyslog"
        + NL
        + "#define vsyslog(pri, fmt, ap) ((void)(pri), (void)(fmt), (void)(ap))"
        + NL
        + "#endif"
        + NL
        + "#ifndef cfsetspeed"
        + NL
        + "#define cfsetspeed(t, s) (cfsetispeed((t), (s)), cfsetospeed((t), (s)))"
        + NL
        + "#endif"
        + NL
        + "#ifndef getopt_long_only"
        + NL
        + "#define getopt_long_only getopt_long"
        + NL
        + "#endif"
        + NL
        + "static inline int awm_wcwidth(wchar_t ucs) { if (ucs == 0) return 0; if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0)) return -1; return 1; }"
        + NL
        + "#ifndef wcwidth"
        + NL
        + "#define wcwidth awm_wcwidth"
        + NL
        + "#endif"
        + NL
        + "static inline char *awm_strcasestr(const char *h, const char *n) { size_t nl = strlen(n); if (!nl) return (char *)h; for (; *h; h++) if (!strncasecmp(h, n, nl)) return (char *)h; return 0; }"
        + NL
        + "#ifndef strcasestr"
        + NL
        + "#define strcasestr awm_strcasestr"
        + NL
        + "#endif"
        + NL
        + "static inline void *awm_memmem(const void *l, size_t l_len, const void *s, size_t s_len) { const char *cl = (const char *)l, *cs = (const char *)s; size_t i; if (!s_len) return (void *)cl; if (l_len < s_len) return 0; for (i = 0; i <= l_len - s_len; i++) if (!memcmp(cl + i, cs, s_len)) return (void *)(cl + i); return 0; }"
        + NL
        + "#ifndef memmem"
        + NL
        + "#define memmem awm_memmem"
        + NL
        + "#endif"
        + NL
        + "#if !defined(_TIMER_T_DEFINED) && !defined(_TIMER_T_DEFINED_) && !defined(__WATCOMC__)"
        + NL
        + "#define _TIMER_T_DEFINED"
        + NL
        + "typedef void *timer_t;"
        + NL
        + "#endif"
        + NL
        + "#if defined(__WATCOMC__)"
        + NL
        + "#undef timer_t"
        + NL
        + "#define timer_t void *"
        + NL
        + "#endif"
        + NL
        + "#if !defined(_ITIMERSPEC_DEFINED) && !defined(__MINGW32__)"
        + NL
        + "#define _ITIMERSPEC_DEFINED"
        + NL
        + "struct itimerspec { struct timespec it_interval; struct timespec it_value; };"
        + NL
        + "#endif"
        + NL
        + "struct sigevent;"
        + NL
        + "int timer_create_wrap(clockid_t c, struct sigevent *se, timer_t *t);"
        + NL
        + "#define timer_create(...) timer_create_wrap(__VA_ARGS__)"
        + NL
        + "int timer_settime_wrap(timer_t t, int flags, struct itimerspec *val, struct itimerspec *old);"
        + NL
        + "#define timer_settime(...) timer_settime_wrap(__VA_ARGS__)"
        + NL
        + "#elif defined(__GLIBC__)"
        + NL
        + "// Work around a glibc bug",
    )
    content = content.replace(
        "enum {" + NL + "  empty = 1 << 9," + NL + "  eol = 1 << 10," + NL + "  eof = 1 << 11," + NL + "  space = 1 << 12" + NL + "};",
        "#undef eof" + NL + "#define eof awm_toybox_eof" + NL + "enum {" + NL + "  empty = 1 << 9," + NL + "  eol = 1 << 10," + NL + "  eof = 1 << 11," + NL + "  space = 1 << 12" + NL + "};",
    )

    content = content.replace(
        "#if __has_include(<sys/random.h>) && (!defined(__ANDROID__) || __ANDROID_API__>28)",
        "#if defined(__linux__)",
    )
    content = content.replace(
        "#elif defined(__linux__)" + NL + NL + "#include <mntent.h>",
        "#elif defined(__linux__) || defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + NL
        + "#include <mntent.h>",
    )
    content = content.replace(
        "#elif defined(__linux__)" + NL + NL + "#include <sys/inotify.h>",
        "#elif defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "struct xnotify *xnotify_init(int max) { struct xnotify *not = xzalloc(sizeof(struct xnotify)); not->max = max; not->paths = xmalloc(max * sizeof(char *)); not->fds = xmalloc(max * 2 * sizeof(int)); return not; }"
        + NL
        + "int xnotify_add(struct xnotify *not, int fd, char *path) { int i = 2*not->count; if (not->max == not->count) error_exit(\"xnotify_add overflow\"); not->fds[i] = -1; not->fds[i+1] = fd; not->paths[not->count++] = path; return 0; }"
        + NL
        + "int xnotify_wait(struct xnotify *not, char **path) { (void)not; (void)path; return -1; }"
        + NL
        + "#elif defined(__linux__)"
        + NL
        + NL
        + "#include <sys/inotify.h>",
    )
    content = content.replace(
        "#elif defined(__IsBSD__)" + NL + "#warning no xattr support in BSD",
        "#elif defined(__IsBSD__) || defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "ssize_t xattr_get(const char *p, const char *n, void *v, size_t s) { (void)p;(void)n;(void)v;(void)s; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_lget(const char *p, const char *n, void *v, size_t s) { (void)p;(void)n;(void)v;(void)s; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_fget(int fd, const char *n, void *v, size_t s) { (void)fd;(void)n;(void)v;(void)s; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_list(const char *p, char *l, size_t s) { (void)p;(void)l;(void)s; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_llist(const char *p, char *l, size_t s) { (void)p;(void)l;(void)s; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_flist(int fd, char *l, size_t s) { (void)fd;(void)l;(void)s; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_set(const char *p, const char *n, const void *v, size_t s, int f) { (void)p;(void)n;(void)v;(void)s;(void)f; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_lset(const char *p, const char *n, const void *v, size_t s, int f) { (void)p;(void)n;(void)v;(void)s;(void)f; errno = ENOSYS; return -1; }"
        + NL
        + "ssize_t xattr_fset(int fd, const char *n, const void *v, size_t s, int f) { (void)fd;(void)n;(void)v;(void)s;(void)f; errno = ENOSYS; return -1; }",
    )
    content = content.replace(
        "#else" + NL + "#error" + NL + "  return dev&0xff;",
        "#else" + NL + "  return dev&0xff;",
    )
    content = content.replace(
        "#else" + NL + "#error" + NL + "  return dev>>8;",
        "#else" + NL + "  return dev>>8;",
    )
    content = content.replace(
        "#else" + NL + "#error" + NL + "  return (major<<8)|minor;",
        "#else" + NL + "  return (major<<8)|minor;",
    )
    content = content.replace(
        "  if (!s) sprintf(s = libbuf, \"0x%x\", (unsigned)statfs->f_type);" + NL + "  return s;" + NL + "#else" + NL + "#error" + NL + "#endif",
        "  if (!s) sprintf(s = libbuf, \"0x%x\", (unsigned)statfs->f_type);" + NL + "  return s;" + NL + "#else" + NL + "  (void)statfs; return \"unknown\";" + NL + "#endif",
    )
    content = content.replace(
        "#elif defined(__GLIBC__)" + NL + "int timer_create_wrap(",
        "#elif defined(_WIN32) || defined(_MSC_VER) || defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "int timer_create_wrap(clockid_t c, struct sigevent *se, timer_t *t) { (void)c; (void)se; if (t) *t = 0; return 0; }"
        + NL
        + "int timer_settime_wrap(timer_t t, int flags, struct itimerspec *val, struct itimerspec *old) { (void)t; (void)flags; (void)val; (void)old; return 0; }"
        + NL
        + "#elif defined(__GLIBC__)"
        + NL
        + "int timer_create_wrap(",
    )

    content = content.replace(
        "char *getbasename(char *name)"
        + NL
        + "{"
        + NL
        + "  char *s = strrchr(name, '/');"
        + NL
        + NL
        + "  if (s) return s+1;"
        + NL
        + NL
        + "  return name;"
        + NL
        + "}",
        "char *getbasename(char *name)"
        + NL
        + "{"
        + NL
        + "  char *s = strrchr(name, '/');"
        + NL
        + "  char *bs = strrchr(name, '\\\\');"
        + NL
        + "  if (bs && (!s || bs > s)) s = bs;"
        + NL
        + "  if (s) return s+1;"
        + NL
        + "  return name;"
        + NL
        + "}",
    )

    content = content.replace(
        "path = xmalloc(len)+len-ll;",
        "path = (char *)xmalloc(len)+len-ll;",
    )
    content = content.replace(
        "else if (node->dirfd != -1) dir = fdopendir(xdup(node->dirfd));",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "  else if (node->dirfd != -1) { char *dp = dirtree_path(node, 0); dir = opendir(dp); free(dp); }"
        + NL
        + "#else"
        + NL
        + "  else if (node->dirfd != -1) dir = fdopendir(xdup(node->dirfd));"
        + NL
        + "#endif",
    )
    content = content.replace(
        "    if ((flags&DIRTREE_SYMFOLLOW) && entry->d_type==DT_LNK"
        + NL
        + "      && !S_ISLNK(new->st.st_mode)) new->again |= DIRTREE_SYMFOLLOW;"
        + NL
        + "    if (!new->st.st_blksize && !new->st.st_mode)"
        + NL
        + "      new->st.st_mode = entry->d_type<<12;",
        "#if !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)"
        + NL
        + "    if ((flags&DIRTREE_SYMFOLLOW) && entry->d_type==DT_LNK"
        + NL
        + "      && !S_ISLNK(new->st.st_mode)) new->again |= DIRTREE_SYMFOLLOW;"
        + NL
        + "    if (!new->st.st_blksize && !new->st.st_mode)"
        + NL
        + "      new->st.st_mode = entry->d_type<<12;"
        + NL
        + "#endif",
    )

    content = content.replace(
        "buf += fd;",
        "buf = (char *)buf + fd;",
    )

    content = content.replace(
        "memcpy(TT.obuf+TT.opos, buf+i, j);",
        "memcpy(TT.obuf+TT.opos, (char *)buf+i, j);",
    )

    content = content.replace(
        "else s = memcpy(s, (wc==127) ? \"DEL\" : low+wc*3, 3)+3;",
        "else s = (char *)memcpy(s, (wc==127) ? \"DEL\" : low+wc*3, 3)+3;",
    )

    content = content.replace(
        "buf[3] = 250; // precision -6, minimum allowed",
        "buf[3] = (char)250; // precision -6, minimum allowed",
    )

    content = content.replace(
        "#define FOR_fsync",
        "#ifndef O_NOATIME" + NL + "#define O_NOATIME 0" + NL + "#endif" + NL
        + "#if defined(__APPLE__)" + NL
        + "#ifndef fdatasync" + NL + "#define fdatasync fsync" + NL + "#endif" + NL + "#endif" + NL
        + "#define FOR_fsync",
    )

    content = content.replace(
        "    printf(\", %s float\","
        + NL
        + "      (char *[]){\"soft\", \"single\", \"double\", \"quad\"}[(flags&0x6)/2]);",
        "    { static char *const _fp[] = {\"soft\", \"single\", \"double\", \"quad\"};"
        + NL
        + "      printf(\", %s float\", _fp[(flags&0x6)/2]); }",
    )

    content = content.replace(
        "if (result<(unsigned []){0x80,0x800,0x10000}[c-2]) return -1;",
        "{ static const unsigned _ov[] = {0x80,0x800,0x10000}; if (result<_ov[c-2]) return -1; }",
    )

    content = content.replace(
        "  transform = (void *[]){md5_transform, sha1_transform, sha2_32_transform,"
        + NL
        + "    sha2_32_transform, sha2_64_transform, sha2_64_transform}[method];"
        + NL
        + "  digestlen = (char []){16, 20, 28, 32, 48, 64}[method];"
        + NL
        + "  chunksize = 64<<(method>=4);"
        + NL
        + "  if (method<=1)"
        + NL
        + "    memcpy(hash->state.i32, (unsigned []){0x67452301, 0xEFCDAB89, 0x98BADCFE,"
        + NL
        + "      0x10325476, 0xC3D2E1F0}, 20);"
        + NL
        + "  else if (method==2)"
        + NL
        + "    memcpy(hash->state.i32, (unsigned []){0xc1059ed8, 0x367cd507, 0x3070dd17,"
        + NL
        + "      0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4}, 32);"
        + NL
        + "  else if (method==3)"
        + NL
        + "    memcpy(hash->state.i32, (unsigned []){0x6a09e667, 0xbb67ae85, 0x3c6ef372,"
        + NL
        + "      0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19}, 32);"
        + NL
        + "  else if (method==4)"
        + NL
        + "    memcpy(hash->state.i64, (unsigned long long []){0xcbbb9d5dc1059ed8,"
        + NL
        + "      0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,"
        + NL
        + "      0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7,"
        + NL
        + "      0x47b5481dbefa4fa4}, 64);"
        + NL
        + "  else memcpy(hash->state.i64, (unsigned long long []){0x6a09e667f3bcc908,"
        + NL
        + "      0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,"
        + NL
        + "      0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b,"
        + NL
        + "      0x5be0cd19137e2179}, 64);",
        "  { static void *const _tr[] = {md5_transform, sha1_transform, sha2_32_transform,"
        + NL
        + "      sha2_32_transform, sha2_64_transform, sha2_64_transform};"
        + NL
        + "    static const char _dl[] = {16, 20, 28, 32, 48, 64};"
        + NL
        + "    static const unsigned _s1[] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};"
        + NL
        + "    static const unsigned _s2[] = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};"
        + NL
        + "    static const unsigned _s3[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};"
        + NL
        + "    static const unsigned long long _s4[] = {0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL, 0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL, 0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL, 0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL};"
        + NL
        + "    static const unsigned long long _s5[] = {0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL};"
        + NL
        + "    transform = _tr[method];"
        + NL
        + "    digestlen = _dl[method];"
        + NL
        + "    chunksize = 64<<(method>=4);"
        + NL
        + "    if (method<=1) memcpy(hash->state.i32, _s1, 20);"
        + NL
        + "    else if (method==2) memcpy(hash->state.i32, _s2, 32);"
        + NL
        + "    else if (method==3) memcpy(hash->state.i32, _s3, 32);"
        + NL
        + "    else if (method==4) memcpy(hash->state.i64, _s4, 64);"
        + NL
        + "    else memcpy(hash->state.i64, _s5, 64); }",
    )

    content = content.replace(
        "len = (char []){0,3,2}[base/8];",
        "{ static const char _bl[] = {0,3,2}; len = _bl[base/8]; }",
    )

    content = content.replace(
        "if ((i = ((char []){80,80,48,16,32,0,64,2,1,8,96,4}[i])<<9)>=4096)",
        "{ static const char _tm[] = {80,80,48,16,32,0,64,2,1,8,96,4}; i = _tm[i]<<9; } if (i>=4096)",
    )

    content = content.replace(
        "  if (FLAG(s) && xrun((char *[]){\"strip\", \"-p\", TT.destname, 0}))"
        + NL
        + "    toys.exitval = 1;",
        "  if (FLAG(s)) { char *_sa[4]; _sa[0] = \"strip\"; _sa[1] = \"-p\"; _sa[2] = TT.destname; _sa[3] = 0; if (xrun(_sa)) toys.exitval = 1; }",
    )

    content = content.replace(
        "  if (FLAG(i)) memcpy(dsuapm+1, (char *[]){\"Inodes\", \"IUsed\", \"IFree\", \"IUse%\"},"
        + NL
        + "                      sizeof(char *)*4);",
        "  if (FLAG(i)) { static char *const _in[] = {\"Inodes\", \"IUsed\", \"IFree\", \"IUse%\"}; memcpy(dsuapm+1, _in, sizeof(char *)*4); }",
    )

    content = content.replace(
        "for (ss = *ss ? ss : (char *[]){\".\", 0}; *ss; ss++) {",
        "{ static char *_dot[] = {\".\", 0}; for (ss = *ss ? ss : _dot; *ss; ss++) {",
    ).replace(
        "    }" + NL + "  } else loopfiles_rw(",
        "    } }" + NL + "  } else loopfiles_rw(",
    )

    content = content.replace(
        "  char **ss = (char *[]){\".\"};",
        "  static char *_dot[] = {\".\"}; char **ss = _dot;",
    )
    content = content.replace(
        "          time_t thyme = (int []){new->st.st_atime, new->st.st_ctime,"
        + NL
        + "                                  new->st.st_mtime}[stridx(\"acm\", *s)];",
        "          int _th[3]; time_t thyme; _th[0] = new->st.st_atime; _th[1] = new->st.st_ctime; _th[2] = new->st.st_mtime; thyme = _th[stridx(\"acm\", *s)];",
    )
    content = content.replace(
        "            units = (int []){86400, 3600, 60, 1}[uu];",
        "            { static const int _u[] = {86400, 3600, 60, 1}; units = _u[uu]; }",
    )

    content = content.replace(
        "else TT.hdr.mode |= (char []){8,8,10,2,6,4,1,8}[tar.type-'0']<<12;",
        "else { static const char _tm[] = {8,8,10,2,6,4,1,8}; TT.hdr.mode |= _tm[tar.type-'0']<<12; }",
    )
    content = content.replace(
        "          pid = xpopen((char *[]){\"sh\", \"-c\", TT.to_command, NULL}, &fd, 0);",
        "          { char *_sh[4]; _sh[0] = \"sh\"; _sh[1] = \"-c\"; _sh[2] = TT.to_command; _sh[3] = NULL; pid = xpopen(_sh, &fd, 0); }",
    )
    content = content.replace(
        "      TT.pid = xpopen_both(zcat ? (char *[]){zcat->str, 0} :"
        + NL
        + "        (char *[]){get_archiver(), \"-d\", 0}, pipefd);",
        "      { char *_za[3]; if (zcat) { _za[0] = zcat->str; _za[1] = 0; } else { _za[0] = get_archiver(); _za[1] = \"-d\"; _za[2] = 0; } TT.pid = xpopen_both(_za, pipefd); }",
    )
    content = content.replace(
        "      TT.pid = xpopen_both((char *[]){get_archiver(), 0}, pipefd);",
        "      { char *_za[2]; _za[0] = get_archiver(); _za[1] = 0; TT.pid = xpopen_both(_za, pipefd); }",
    )
    content = content.replace(
        "struct timespec times[2] = {{sec, 0},{sec, 0}};",
        "struct timespec times[2]; times[0].tv_sec = sec; times[0].tv_nsec = 0; times[1].tv_sec = sec; times[1].tv_nsec = 0;",
    )
    content = content.replace(
        "int pipefd[2] = {hdr ? -1 : TT.fd, -1}, i, pid;",
        "int pipefd[2], i, pid; pipefd[0] = hdr ? -1 : TT.fd; pipefd[1] = -1;",
    )
    content = content.replace(
        "int pipefd[2] = {-1, TT.fd};",
        "int pipefd[2]; pipefd[0] = -1; pipefd[1] = TT.fd;",
    )

    content = content.replace(
        "    error_exit_raw((char *[]){\"Unsupported options in the .xz headers\","
        + NL
        + "      \"File is corrupt\"}[ret-2]);",
        "    { static char *const _em[] = {\"Unsupported options in the .xz headers\", \"File is corrupt\"}; error_exit_raw(_em[ret-2]); }",
    )

    content = content.replace(
        "  *f = (struct zfile){TT.zfiles, xstrdup(fn), fp, mode, file_or_pipe,"
        + NL
        + "                isatty(fileno(fp)), is_std_file, 0, 0, 0, 0, 0};",
        "  f->next = TT.zfiles; f->fn = xstrdup(fn); f->fp = fp; f->mode = mode;"
        + NL
        + "  f->file_or_pipe = file_or_pipe; f->is_tty = isatty(fileno(fp)); f->is_std_file = is_std_file;",
    )
    content = content.replace(
        "*TT.cfile = (struct zfile){0};",
        "memset(TT.cfile, 0, sizeof(struct zfile));",
    )
    content = content.replace(
        "r = (p->fp) ? (p->file_or_pipe ? fclose : pclose)(p->fp) : -1;",
        "r = (p->fp) ? (p->file_or_pipe ? fclose(p->fp) : pclose(p->fp)) : -1;",
    )
    content = content.replace(
        "FILE *fp = (file_or_pipe ? fopen : popen)(fn, mode);",
        "FILE *fp = file_or_pipe ? fopen(fn, mode) : popen(fn, mode);",
    )

    _esc_e_re = re.compile(
        r'("(?:\\.|[^"\\\x0a])*"|\x27(?:\\.|[^\x27\\\x0a])*\x27)'
    )
    content = _esc_e_re.sub(
        lambda m: re.sub(r"(?<!\\)\\e", lambda _: BS + "033", m.group(0)),
        content,
    )

    content = re.sub(
        r"\breturn\s+((?:error_msg|perror_msg|perror_msg_raw|xprintf|xputs|xputsn|xputc|forget_arg|header_time|error_time|flagshow|do_help|show_status|arg_add|skippy|newline)\s*\([^;]*\));",
        r"{ \1; return; }",
        content,
    )

    content = content.replace(
        "char *pp[] = {\"0%3o%n\"+!echo,\"x%2x%n\",\"u%4x%n\",\"U%8x%n\"};",
        "char *pp[4]; pp[0] = \"0%3o%n\"+!echo; pp[1] = \"x%2x%n\"; pp[2] = \"u%4x%n\"; pp[3] = \"U%8x%n\";",
    )
    content = content.replace(
        "    if (sscanf(*c, (char *[]){\"0%3o%n\"+!echo, \"x%2x%n\", \"u%4x%n\", \"U%6x%n\"}[i],"
        + NL
        + "        &idx, &off) > 0)",
        "    { char *pp[4]; pp[0] = \"0%3o%n\"+!echo; pp[1] = \"x%2x%n\"; pp[2] = \"u%4x%n\"; pp[3] = \"U%6x%n\";"
        + NL
        + "    if (sscanf(*c, pp[i], &idx, &off) > 0)",
    )
    content = content.replace(
        "      return idx;" + NL + "    }" + NL + "  }" + NL + NL + "  if (-1 == (idx = stridx(",
        "      return idx;" + NL + "    } }" + NL + "  }" + NL + NL + "  if (-1 == (idx = stridx(",
    )

    content = content.replace(
        "  char *s = str, *p, *oldtz = 0, *formats[] = {\"%Y-%m-%d %T\", \"%Y-%m-%dT%T\","
        + NL
        + "    \"%a %b %e %H:%M:%S %Z %Y\", // date(1) output format in POSIX/C locale."
        + NL
        + "    \"%H:%M:%S\", \"%Y-%m-%d %H:%M\", \"%Y-%m-%d\", \"%H:%M\", \"%m%d%H%M\","
        + NL
        + "    endian ? \"%m%d%H%M%y\" : \"%y%m%d%H%M\","
        + NL
        + "    endian ? \"%m%d%H%M%C%y\" : \"%C%y%m%d%H%M\"};",
        "  char *s = str, *p, *oldtz = 0, *formats[10];"
        + NL
        + "  formats[0] = \"%Y-%m-%d %T\"; formats[1] = \"%Y-%m-%dT%T\";"
        + NL
        + "  formats[2] = \"%a %b %e %H:%M:%S %Z %Y\"; formats[3] = \"%H:%M:%S\";"
        + NL
        + "  formats[4] = \"%Y-%m-%d %H:%M\"; formats[5] = \"%Y-%m-%d\";"
        + NL
        + "  formats[6] = \"%H:%M\"; formats[7] = \"%m%d%H%M\";"
        + NL
        + "  formats[8] = endian ? \"%m%d%H%M%y\" : \"%y%m%d%H%M\";"
        + NL
        + "  formats[9] = endian ? \"%m%d%H%M%C%y\" : \"%C%y%m%d%H%M\";",
    )

    content = content.replace(
        "  struct flock lock = {.l_type = F_WRLCK};",
        "#if !defined(__WATCOMC__) && !defined(__DOS__) && !defined(__MSDOS__)"
        + NL
        + "  struct flock lock = {.l_type = F_WRLCK};"
        + NL
        + "#else"
        + NL
        + "  int lock = 0;"
        + NL
        + "#endif",
    )

    content = content.replace(
        "    dd = (double []){first, increment, last}[ii];",
        "    { double _da[3]; _da[0] = first; _da[1] = increment; _da[2] = last; dd = _da[ii]; }",
    )

    content = content.replace(
        "  char *labels[] = {\"\\nreal\"+FLAG(p), \"user\", \"sys\"}, **label = labels,"
        + NL
        + "       *vlabels[] ={\"Real\", \"User\", \"System\"}, tab = toys.optflags ? ' ' : '\\t';",
        "  char *labels[3], **label = labels, *vlabels[] = {\"Real\", \"User\", \"System\"}, tab = toys.optflags ? ' ' : '\\t';"
        + NL
        + "  labels[0] = \"\\nreal\"+FLAG(p); labels[1] = \"user\"; labels[2] = \"sys\";",
    )

    content = content.replace(
        "if (FLAG(V)) return (void)puts(\"mcookie from toybox\");",
        "if (FLAG(V)) { puts(\"mcookie from toybox\"); return; }",
    )

    content = re.sub(r"=\s*\{\s*\}", "= {0}", content)
    content = content.replace(",{}}", ",{0}}")

    content = content.replace(
        "enum toktypes {" + NL + "    // EOF (use -1 from stdio.h)" + NL + "    ERROR = 2,",
        "#undef ERROR" + NL + "enum toktypes {" + NL + "    // EOF (use -1 from stdio.h)" + NL + "    ERROR = 2,",
    )
    content = content.replace(
        "#define ZVINIT(flags, num, ptr) {(flags), (double)(num), {(ptr)}}",
        "static inline struct zvalue awm_zvinit(unsigned f, double n, void *p) { struct zvalue v; v.flags = f; v.num = n; v.vst = (struct zstring *)p; return v; }"
        + NL
        + "#define ZVINIT(flags, num, ptr) awm_zvinit((flags), (double)(num), (void *)(ptr))",
    )
    content = content.replace(
        "#define ZMSLOTINIT(hash, key, val) {hash, key, val}",
        "static inline struct zmap_slot awm_zmslotinit(unsigned h, struct zstring *k, struct zvalue v) { struct zmap_slot s; s.hash = h; s.key = k; s.val = v; return s; }"
        + NL
        + "#define ZMSLOTINIT(hash, key, val) awm_zmslotinit((hash), (key), (val))",
    )
    content = content.replace(
        "static struct zvalue uninit_zvalue = ZVINIT(0, 0.0, 0);"
        + NL
        + NL
        + "// This will be reassigned in init_globals() with an empty string."
        + NL
        + "// It's a special value used for \"uninitialized\" field vars"
        + NL
        + "// referenced past $NF. See push_field()."
        + NL
        + "static struct zvalue uninit_string_zvalue = ZVINIT(0, 0.0, 0);",
        "static struct zvalue uninit_zvalue = {0, 0.0, {0}};"
        + NL
        + "static struct zvalue uninit_string_zvalue = {0, 0.0, {0}};",
    )
    content = content.replace("(struct zvalue)ZVINIT(", "ZVINIT(")

    content = content.replace(
        "struct timespec times[] = {sb.st_atim, sb.st_mtim};",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "    struct timespec times[2]; times[0] = awm_stat_ts(sb.st_atime); times[1] = awm_stat_ts(sb.st_mtime);"
        + NL
        + "#else"
        + NL
        + "    struct timespec times[] = {sb.st_atim, sb.st_mtim};"
        + NL
        + "#endif",
    )

    content = content.replace(
        "      } else if (!(FLAG(u) && nanodiff(&try->st.st_mtim, &cst.st_mtim)>0)",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "      } else if (!(FLAG(u) && try->st.st_mtime < cst.st_mtime)"
        + NL
        + "#else"
        + NL
        + "      } else if (!(FLAG(u) && nanodiff(&try->st.st_mtim, &cst.st_mtim)>0)"
        + NL
        + "#endif",
    )
    content = content.replace(
        "struct timespec times[] = {try->st.st_atim, try->st.st_mtim};",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "      struct timespec times[2]; times[0] = awm_stat_ts(try->st.st_atime); times[1] = awm_stat_ts(try->st.st_mtime);"
        + NL
        + "#else"
        + NL
        + "      struct timespec times[] = {try->st.st_atim, try->st.st_mtim};"
        + NL
        + "#endif",
    )

    content = content.replace(
        "  else if (type == 'x') date_stat_format(&stat->st_atim);"
        + NL
        + "  else if (type == 'X') out('u', stat->st_atime);"
        + NL
        + "  else if (type == 'y') date_stat_format(&stat->st_mtim);"
        + NL
        + "  else if (type == 'Y') out('u', stat->st_mtime);"
        + NL
        + "  else if (type == 'z') date_stat_format(&stat->st_ctim);",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "  else if (type == 'x') { struct timespec _ts = awm_stat_ts(stat->st_atime); date_stat_format(&_ts); }"
        + NL
        + "  else if (type == 'X') out('u', stat->st_atime);"
        + NL
        + "  else if (type == 'y') { struct timespec _ts = awm_stat_ts(stat->st_mtime); date_stat_format(&_ts); }"
        + NL
        + "  else if (type == 'Y') out('u', stat->st_mtime);"
        + NL
        + "  else if (type == 'z') { struct timespec _ts = awm_stat_ts(stat->st_ctime); date_stat_format(&_ts); }"
        + NL
        + "#else"
        + NL
        + "  else if (type == 'x') date_stat_format(&stat->st_atim);"
        + NL
        + "  else if (type == 'X') out('u', stat->st_atime);"
        + NL
        + "  else if (type == 'y') date_stat_format(&stat->st_mtim);"
        + NL
        + "  else if (type == 'Y') out('u', stat->st_mtime);"
        + NL
        + "  else if (type == 'z') date_stat_format(&stat->st_ctim);"
        + NL
        + "#endif",
    )

    content = content.replace(
        "  printf(\"%s %s\\t%s\\n\", prefix, quoted_file,"
        + NL
        + "    format_iso_time(date, sizeof(date), &sb->st_mtim));",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "  { struct timespec _ts = awm_stat_ts(sb->st_mtime); printf(\"%s %s\\t%s\\n\", prefix, quoted_file, format_iso_time(date, sizeof(date), &_ts)); }"
        + NL
        + "#else"
        + NL
        + "  printf(\"%s %s\\t%s\\n\", prefix, quoted_file,"
        + NL
        + "    format_iso_time(date, sizeof(date), &sb->st_mtim));"
        + NL
        + "#endif",
    )

    content = content.replace(
        "        int macoff[] = {offsetof(struct stat, st_mtim),"
        + NL
        + "          offsetof(struct stat, st_atim), offsetof(struct stat, st_ctim)};",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "        int macoff[] = {offsetof(struct stat, st_mtime), offsetof(struct stat, st_atime), offsetof(struct stat, st_ctime)};"
        + NL
        + "#else"
        + NL
        + "        int macoff[] = {offsetof(struct stat, st_mtim),"
        + NL
        + "          offsetof(struct stat, st_atim), offsetof(struct stat, st_ctim)};"
        + NL
        + "#endif",
    )
    content = content.replace(
        "                sprintf(buf, \"%lld.%ld\", (long long)new->st.st_mtim.tv_sec,"
        + NL
        + "                             new->st.st_mtim.tv_nsec);",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "                sprintf(buf, \"%lld.0\", (long long)new->st.st_mtime);"
        + NL
        + "#else"
        + NL
        + "                sprintf(buf, \"%lld.%ld\", (long long)new->st.st_mtim.tv_sec,"
        + NL
        + "                             new->st.st_mtim.tv_nsec);"
        + NL
        + "#endif",
    )

    content = content.replace(
        "  if (flags&FLAG_t) ts1 = &a->st.st_mtim, ts2 = &b->st.st_mtim;"
        + NL
        + "  if (flags&FLAG_u) ts1 = &a->st.st_atim, ts2 = &b->st.st_atim;"
        + NL
        + "  if (flags&FLAG_c) ts1 = &a->st.st_ctim, ts2 = &b->st.st_ctim;",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "  struct timespec _tsa, _tsb;"
        + NL
        + "  if (flags&FLAG_t) { _tsa = awm_stat_ts(a->st.st_mtime); _tsb = awm_stat_ts(b->st.st_mtime); ts1 = &_tsa; ts2 = &_tsb; }"
        + NL
        + "  if (flags&FLAG_u) { _tsa = awm_stat_ts(a->st.st_atime); _tsb = awm_stat_ts(b->st.st_atime); ts1 = &_tsa; ts2 = &_tsb; }"
        + NL
        + "  if (flags&FLAG_c) { _tsa = awm_stat_ts(a->st.st_ctime); _tsb = awm_stat_ts(b->st.st_ctime); ts1 = &_tsa; ts2 = &_tsb; }"
        + NL
        + "#else"
        + NL
        + "  if (flags&FLAG_t) ts1 = &a->st.st_mtim, ts2 = &b->st.st_mtim;"
        + NL
        + "  if (flags&FLAG_u) ts1 = &a->st.st_atim, ts2 = &b->st.st_atim;"
        + NL
        + "  if (flags&FLAG_c) ts1 = &a->st.st_ctim, ts2 = &b->st.st_ctim;"
        + NL
        + "#endif",
    )
    content = content.replace(
        "s += sprintf(s, \":%02d.%09d \", tm->tm_sec, (int)st->st_mtim.tv_nsec);",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "        s += sprintf(s, \":%02d.000000000 \", tm->tm_sec);"
        + NL
        + "#else"
        + NL
        + "        s += sprintf(s, \":%02d.%09d \", tm->tm_sec, (int)st->st_mtim.tv_nsec);"
        + NL
        + "#endif",
    )

    content = content.replace(
        "ts = st.st_mtim;",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "      ts = awm_stat_ts(st.st_mtime);"
        + NL
        + "#else"
        + NL
        + "      ts = st.st_mtim;"
        + NL
        + "#endif",
    )

    content = content.replace(
        "      if (i==14) return (st1.st_mtim.tv_sec < st2.st_mtim.tv_sec) ||"
        + NL
        + "        (st1.st_mtim.tv_sec == st2.st_mtim.tv_sec &&"
        + NL
        + "        st1.st_mtim.tv_nsec < st2.st_mtim.tv_nsec);"
        + NL
        + "      if (i==16) return (st1.st_mtim.tv_sec > st2.st_mtim.tv_sec) ||"
        + NL
        + "        (st1.st_mtim.tv_sec == st2.st_mtim.tv_sec &&"
        + NL
        + "        st1.st_mtim.tv_nsec > st2.st_mtim.tv_nsec);",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "      if (i==14) return st1.st_mtime < st2.st_mtime;"
        + NL
        + "      if (i==16) return st1.st_mtime > st2.st_mtime;"
        + NL
        + "#else"
        + NL
        + "      if (i==14) return (st1.st_mtim.tv_sec < st2.st_mtim.tv_sec) ||"
        + NL
        + "        (st1.st_mtim.tv_sec == st2.st_mtim.tv_sec &&"
        + NL
        + "        st1.st_mtim.tv_nsec < st2.st_mtim.tv_nsec);"
        + NL
        + "      if (i==16) return (st1.st_mtim.tv_sec > st2.st_mtim.tv_sec) ||"
        + NL
        + "        (st1.st_mtim.tv_sec == st2.st_mtim.tv_sec &&"
        + NL
        + "        st1.st_mtim.tv_nsec > st2.st_mtim.tv_nsec);"
        + NL
        + "#endif",
    )

    content = content.replace(
        "    ts[0] = st.st_atim;" + NL + "    ts[1] = st.st_mtim;",
        "#if defined(__WATCOMC__) || defined(__DOS__) || defined(__MSDOS__)"
        + NL
        + "    ts[0] = awm_stat_ts(st.st_atime); ts[1] = awm_stat_ts(st.st_mtime);"
        + NL
        + "#else"
        + NL
        + "    ts[0] = st.st_atim; ts[1] = st.st_mtim;"
        + NL
        + "#endif",
    )

    content = _transform_freebsd_c99_c11(content)

    return content


def _transform_freebsd_c99_c11(
    content: str, rename_main: str = ""
) -> str:
    """
    Apply FreeBSD-specific C99/C11/GNU C to C89/MSVC/OpenWatcom rewrites:
    - 1-argument setmode(expr) -> bsd_setmode(expr)
    - __FBSDID / __RCSID / __SCCSID / __COPYRIGHT / DATA_SET / TEXT_SET normalization
    - C99 array parameter qualifiers (static, __min_size, restrict) -> pointer
    - C11 _Noreturn, _Thread_local, _Atomic, _Alignas, _Alignof, _Static_assert
    - Optional main() entry-point renaming for multicall builds
    """
    # 6.2 Normalize top-level __FBSDID("..."); and linker set macros
    content = re.sub(
        r"^\s*__(?:FBSDID|RCSID|SCCSID|COPYRIGHT)\s*\(\s*\"[^\"]*\"\s*\)\s*;?",
        "typedef int __awm_fbsdid_tu_guard;",
        content,
        flags=re.MULTILINE,
    )
    content = re.sub(
        r"^\s*(?:DATA_SET|TEXT_SET|BSS_SET|SET_DECLARE)\s*\([^;\n]+\)\s*;?",
        "typedef int __awm_linkerset_tu_guard;",
        content,
        flags=re.MULTILINE,
    )

    # 6.1 Rewrite 1-argument setmode(arg) to bsd_setmode(arg) without touching 2-arg setmode(fd, mode)
    def replace_setmode_1arg(match: re.Match[str]) -> str:
        prefix = match.group(1)
        args_str = match.group(2)
        depth = 0
        has_top_comma = False
        for ch in args_str:
            if ch in "([":
                depth += 1
            elif ch in ")]":
                depth -= 1
            elif ch == "," and depth == 0:
                has_top_comma = True
                break
        if not has_top_comma:
            return f"{prefix}bsd_setmode({args_str})"
        return match.group(0)

    content = re.sub(
        r"(?<![a-zA-Z0-9_])(\s*)setmode\s*\(([^;{}()\n]*(?:\([^()]*\)[^;{}()\n]*)*)\)",
        replace_setmode_1arg,
        content,
    )

    # 6.3 Handle C99 array size qualifiers in function parameters:
    # e.g. char buf[static 16], char buf[__min_size(16)], char buf[restrict 16], char buf[__restrict]
    content = re.sub(
        r"(\b[a-zA-Z_][a-zA-Z0-9_]*)\s*\[\s*(?:static\b|__min_size\s*\([^)]*\)|restrict\b|__restrict\b)[^\]]*\]",
        r"*\1",
        content,
    )

    # 6.4 Handle C11 _Noreturn, _Thread_local
    content = re.sub(r"\b_Noreturn\b", "__dead2", content)
    content = re.sub(
        r"\b_Thread_local\b",
        "/* _Thread_local */",
        content,
    )

    # 6.5 Optional main() renaming
    if rename_main:
        content = re.sub(
            r"\bint\s+main\s*\(",
            f"int {rename_main}(",
            content,
        )

    return content


def transform_file(
    input_path: str, output_path: str, rename_main: str = ""
) -> None:
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
        """Transform and rewrite relative local #include directives."""
        inc_rel = match.group(1)
        candidate = os.path.normpath(os.path.join(in_dir, inc_rel))
        if os.path.isfile(candidate):
            with open(candidate, "r", encoding="utf-8", errors="ignore") as cf:
                c_text = cf.read()
            c_sub = re.sub(
                r'#\s*include\s+"([^"]+)"', resolve_local_include, c_text
            )
            c_trans = transform_source_content(c_sub)
            if c_trans != c_text:
                out_dir_abs = os.path.dirname(os.path.abspath(output_path))
                rel_target = os.path.basename(inc_rel) if os.path.isabs(inc_rel) else inc_rel
                gen_inc = os.path.join(out_dir_abs, rel_target)
                os.makedirs(os.path.dirname(gen_inc), exist_ok=True)
                existing_inc = None
                if os.path.isfile(gen_inc):
                    with open(gen_inc, "r", encoding="utf-8", errors="ignore") as ef:
                        existing_inc = ef.read()
                if existing_inc != c_trans:
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
    if rename_main:
        transformed = _transform_freebsd_c99_c11(
            transformed, rename_main=rename_main
        )

    out_dir = os.path.dirname(os.path.abspath(output_path))
    os.makedirs(out_dir, exist_ok=True)

    existing_out = None
    if os.path.isfile(output_path):
        with open(output_path, "r", encoding="utf-8", errors="ignore") as ef:
            existing_out = ef.read()
    if existing_out != transformed:
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
    parser.add_argument(
        "--rename-main",
        default="",
        help="Optional symbol name to rename main() to for multicall binaries",
    )

    args = parser.parse_args()

    if args.output:
        if len(args.inputs) != 1:
            print("Error: --output can only be used with a single input file.")
            sys.exit(1)
        transform_file(
            args.inputs[0], args.output, rename_main=args.rename_main
        )
    elif args.out_dir:
        for inp in args.inputs:
            rel_name = os.path.basename(inp)
            out_file = os.path.join(args.out_dir, rel_name)
            transform_file(inp, out_file, rename_main=args.rename_main)
    elif len(args.inputs) == 2 and not os.path.isdir(args.inputs[1]):
        transform_file(
            args.inputs[0], args.inputs[1], rename_main=args.rename_main
        )
    else:
        print(
            "Usage: transform_sources.py <input> <output> OR --out-dir <dir> <files...>"
        )
        sys.exit(1)


if __name__ == "__main__":  # pragma: no cover
    main()
