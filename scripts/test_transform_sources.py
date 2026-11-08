#!/usr/bin/env python3
"""
Unit tests for scripts/transform_sources.py achieving 100% coverage.
"""

import os
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import transform_sources


class TestTransformSources(unittest.TestCase):
    """Test suite for C89/MSVC source transformation engine."""

    def test_vla_and_alloca_injection(self) -> None:
        """Verify dynamic VLAs are rewritten to alloca() and constant arrays are untouched."""
        nl = chr(10)
        src = nl.join(
            [
                "void foo(int n) {",
                "  char buf[n + 1];",
                "  char fixed[64];",
                "  int sz[sizeof(int)];",
                "}",
            ]
        )
        res = transform_sources.transform_source_content(src)
        self.assertIn("char *buf = (char *)alloca((n + 1) * sizeof(char));", res)
        self.assertIn("char fixed[64];", res)
        self.assertIn("int sz[sizeof(int)];", res)
        self.assertIn("#include <malloc.h>", res)

    def test_elvis_and_self_init(self) -> None:
        """Verify GNU ?: operator and x = x self-initialization are rewritten."""
        nl = chr(10)
        src = nl.join(
            [
                "void bar(const char *a) {",
                "  int x = x;",
                "  char *p = p;",
                '  const char *r = a ?: "default";',
                "}",
            ]
        )
        res = transform_sources.transform_source_content(src)
        self.assertIn("int x = 0;", res)
        self.assertIn("char *p = 0;", res)
        self.assertIn('(a ? a : "default")', res)

    def test_jq_c89_transforms(self) -> None:
        """Verify jq C99 jv compound literals and designated array initializers are rewritten to C89."""
        nl = chr(10)
        src = nl.join(
            [
                "__attribute__((warn_unused_result)) void* jv_mem_realloc(void*, size_t);",
                "typedef struct {",
                "  unsigned char kind_flags;",
                "} jv;",
                "jv make_null(void) {",
                "  return (jv){JVP_FLAGS_NULL, 0, 0, 0, {0}};",
                "}",
                "jv make_ptr(struct jv_refcnt *p) {",
                "  return (jv){JVP_FLAGS_ARRAY, 0, 0, 1, {.ptr = p}};",
                "}",
                "jv make_num(double d) {",
                "  return (jv){JVP_FLAGS_NUMBER, 0, 0, 0, {.number = d}};",
                "}",
                "static const char *const names[] = {",
                '  [JV_KIND_INVALID] = "<invalid>",',
                '  [JV_KIND_NULL] = "null",',
                "};",
                "#define MIN(a,b) \\",
                "  ({ __typeof__ (a) _a = (a); \\",
                "   __typeof__ (b) _b = (b); \\",
                "   _a < _b ? _a : _b; })",
                "#define MAX(a,b) \\",
                "  ({ __typeof__ (a) _a = (a); \\",
                "   __typeof__ (b) _b = (b); \\",
                "   _a > _b ? _a : _b; })",
                "#define BLOCK_IDX(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME",
                "#define BLOCK(...) \\",
                "  BLOCK_IDX(__VA_ARGS__, BLOCK_8, BLOCK_7, BLOCK_6, BLOCK_5, BLOCK_4, BLOCK_3, BLOCK_2, BLOCK_1)(__VA_ARGS__)",
                "void ptr_arith(void *tm, int *offsets, int i) {",
                "  *(int *)((void *)tm + offsets[i]) = 0;",
                "}",
            ]
        )
        res = transform_sources.transform_source_content(src)
        self.assertIn("#define __attribute__(x)", res)
        self.assertIn("awm_jv_make_ptr(JVP_FLAGS_NULL, 0, 0, 0, 0)", res)
        self.assertIn("awm_jv_make_ptr(JVP_FLAGS_ARRAY, 0, 0, 1, p)", res)
        self.assertIn("awm_jv_make_num(JVP_FLAGS_NUMBER, 0, 0, 0, d)", res)
        self.assertNotIn("[JV_KIND_NULL]", res)
        self.assertIn("#define MIN(a,b) ((a) < (b) ? (a) : (b))", res)
        self.assertIn("#define MAX(a,b) ((a) > (b) ? (a) : (b))", res)
        self.assertIn("AWM_VA_APPLY(BLOCK_IDX,", res)
        self.assertIn("((char *)tm + offsets[i])", res)
        minrx_src = nl.join(
            [
                '#include "charset.c"',
                "return LITERAL(QVecInsert) {newly, &q->storage[k]};",
                "return LITERAL(Subexp) {{NULL, NULL, 0}, 0, false};",
                "return mkrep(c, LITERAL(Subexp) {lh.nodes, rh.maxstk, rh.hasmin}, true, false, nstk);",
                "return LITERAL(Subexp) {lh.nodes, lh.maxstk, lh.hasmin};",
            ]
        )
        minrx_res = transform_sources.transform_source_content(minrx_src)
        self.assertIn("#undef min", minrx_res)
        self.assertIn("QVecInsert _t;", minrx_res)

    def test_packed_structs(self) -> None:
        """Verify PACKED structs are wrapped in MSVC pragma pack."""
        nl = chr(10)
        src = nl.join(
            [
                "struct foo {",
                "  int a;",
                "} PACKED;",
                "struct multi",
                "{",
                "  int m;",
                "} PACKED;",
                "struct fwd",
                "  int x;",
                "struct normal {",
                "  int b;",
                "};",
            ]
        )
        res = transform_sources.transform_source_content(src)
        self.assertIn("#pragma pack(push, 1)", res)
        self.assertIn("#pragma pack(pop)", res)

    def test_statement_expressions_and_collisions(self) -> None:
        """Verify platform.h / libbb.h statement expressions and enum collisions are transformed."""
        nl = chr(10)
        tab = chr(9)
        src = nl.join(
            [
                "#elif defined(__i386__)",
                "# define BB_BIG_ENDIAN 0",
                "#define get_unaligned_le32(buf) ({ uint32_t v; move_from_unaligned32(v, buf); SWAP_LE32(v); })",
                "#define get_unaligned_be32(buf) ({ uint32_t v; move_from_unaligned32(v, buf); SWAP_BE32(v); })",
                "#define get_le32(u32p) ({ uint32_t v = *(bb__aliased_uint32_t*)(u32p); SWAP_LE32(v); })",
                "#define isspace(a) ({ unsigned char bb__isspace = (a) - 9; bb__isspace == (' ' - 9) || bb__isspace <= (13 - 9); })",
                '#define barrier() asm volatile ("":::"memory")',
                "void bb_die(const char *s) NORETURN FAST_FUNC;",
                tab + "TRUE, FALSE",
                tab + "PIPE_BUF = 4096,",
                "#if BUILD_AS_NOMMU",
                "sigjmp_buf env;",
                "",
            ]
        )
        res = transform_sources.transform_source_content(src)
        self.assertIn("defined(_WIN32) || defined(_MSC_VER)", res)
        self.assertIn("bb_get_unaligned_le32", res)
        self.assertIn("bb_get_le32", res)
        self.assertIn("bb_ascii_isspace", res)
        self.assertIn("_ReadWriteBarrier", res)
        self.assertIn("NORETURN void bb_die(const char *s) FAST_FUNC;", res)
        self.assertIn("#undef TRUE", res)
        self.assertIn("#undef PIPE_BUF", res)
        self.assertIn("#if BUILD_AS_NOMMU || defined(_WIN32)", res)
        bb_src = nl.join(
            [
                "#define xstrtoul_range_sfx(str, l, u, sfx) 0",
                "#elif UINT_MAX == ULONG_MAX",
                "DEFINE_EQUIV_STR_CONV(int, i, l, u, ul)",
                "#define fork_transformer fork_transformer_mmu",
                "int FAST_FUNC setup_unzip_on_fd(int fd, int die_if_not_compressed)",
                "{",
                tab + "transformer_state_t *xstate = setup_transformer_on_fd(fd, die_if_not_compressed);",
                tab + "fork_transformer_and_free(xstate);",
                tab + "return 0;",
                "}",
            ]
        )
        bb_res = transform_sources.transform_source_content(bb_src)
        self.assertIn("#elif 0", bb_res)
        self.assertIn("unpack_gz_stream", bb_res)

    def test_file_transform_and_local_includes(self) -> None:
        """Verify transform_file and CLI main branches."""
        nl = chr(10)
        with tempfile.TemporaryDirectory() as tmpdir:
            sub_hdr = os.path.join(tmpdir, "local_sub.h")
            with open(sub_hdr, "w", encoding="utf-8") as f:
                f.write(nl.join(["struct s {", "  int x;", "} PACKED;", ""]))
            plain_hdr = os.path.join(tmpdir, "plain_sub.h")
            with open(plain_hdr, "w", encoding="utf-8") as f:
                f.write("int plain_x;" + nl)
            in_c = os.path.join(tmpdir, "test_in.c")
            with open(in_c, "w", encoding="utf-8") as f:
                f.write(
                    nl.join(
                        [
                            '#include "local_sub.h"',
                            '#include "plain_sub.h"',
                            '#include "nonexistent.h"',
                            "int main(void) { return 0; }",
                            "",
                        ]
                    )
                )
            out_c = os.path.join(tmpdir, "out", "test_in.c")
            transform_sources.transform_file(in_c, out_c)
            transform_sources.transform_file(in_c, out_c)
            self.assertTrue(os.path.isfile(out_c))

            out_h = os.path.join(tmpdir, "out", "local_sub.h")
            transform_sources.transform_file(sub_hdr, out_h)
            self.assertTrue(os.path.isfile(out_h))

            out_c2 = os.path.join(tmpdir, "out2.c")
            with mock.patch.object(
                sys, "argv", ["transform_sources.py", in_c, "-o", out_c2]
            ):
                transform_sources.main()
            self.assertTrue(os.path.isfile(out_c2))

            with mock.patch.object(
                sys,
                "argv",
                ["transform_sources.py", in_c, plain_hdr, "-o", out_c2],
            ), self.assertRaises(SystemExit):
                transform_sources.main()

            out_dir = os.path.join(tmpdir, "outdir")
            with mock.patch.object(
                sys, "argv", ["transform_sources.py", "-d", out_dir, in_c]
            ):
                transform_sources.main()
            self.assertTrue(os.path.isfile(os.path.join(out_dir, "test_in.c")))

            out_c3 = os.path.join(tmpdir, "out3.c")
            with mock.patch.object(
                sys, "argv", ["transform_sources.py", in_c, out_c3]
            ):
                transform_sources.main()
            self.assertTrue(os.path.isfile(out_c3))

            with mock.patch.object(
                sys, "argv", ["transform_sources.py", in_c]
            ), self.assertRaises(SystemExit):
                transform_sources.main()


if __name__ == "__main__":  # pragma: no cover
    unittest.main()
