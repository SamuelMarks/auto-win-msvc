"""Comprehensive unit tests for standalone Python utility scripts."""

import importlib.util
import io
import json
import os
import runpy
import subprocess
import sys
import tempfile
import unittest
from unittest import mock


REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
NL = chr(10)
BS = chr(92)
DQ = chr(34)
SQ = chr(39)


def _load_module(name: str, rel_path: str):
    abs_path = os.path.join(REPO_ROOT, rel_path)
    spec = importlib.util.spec_from_file_location(name, abs_path)
    assert spec is not None and spec.loader is not None
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod, abs_path


class TestStandalonePythonScripts(unittest.TestCase):
    """Tests covering jq_headers.py and scripts/*.py helpers."""

    def test_jq_headers(self):
        mod, abs_path = _load_module("jq_headers_mod", "cmake/jq-port/jq_headers.py")
        self.assertTrue(callable(mod.main))

        with tempfile.TemporaryDirectory() as tmp:
            builtin_jq = os.path.join(tmp, "builtin.jq")
            gen_dir = os.path.join(tmp, "gen")
            with open(builtin_jq, "wb") as bf:
                bf.write(
                    ("def foo: .bar;" + NL + "def baz: .qux;" + NL).encode("utf-8")
                )

            with mock.patch.object(sys, "argv", [abs_path, builtin_jq, gen_dir]):
                runpy.run_path(abs_path, run_name="__main__")

            self.assertTrue(os.path.isfile(os.path.join(gen_dir, "version.h")))
            self.assertTrue(os.path.isfile(os.path.join(gen_dir, "src", "version.h")))
            self.assertTrue(os.path.isfile(os.path.join(gen_dir, "config.h")))
            self.assertTrue(os.path.isfile(os.path.join(gen_dir, "builtin.inc")))
            self.assertTrue(os.path.isfile(os.path.join(gen_dir, "src", "builtin.inc")))

            empty_jq = os.path.join(tmp, "empty.jq")
            with open(empty_jq, "wb") as ef:
                ef.write(b"")
            with mock.patch.object(sys, "argv", [abs_path, empty_jq, gen_dir]):
                mod.main()

    def test_check_c89_strict(self):
        mod, abs_path = _load_module(
            "check_c89_strict_mod", "scripts/check_c89_strict.py"
        )

        with tempfile.TemporaryDirectory() as tmp:
            os.makedirs(os.path.join(tmp, "mod_a", "src"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "mod_a", "include"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "mod_b", "include", "sub"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "build_tmp", "include"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "downstream_test", "include"), exist_ok=True)
            os.makedirs(os.path.join(tmp, ".git", "include"), exist_ok=True)

            src_c = os.path.join(".", "mod_a", "src", "ok.c")
            root_c = "root.c"
            with open(os.path.join(tmp, "mod_a", "src", "ok.c"), "w") as f:
                f.write("int x;" + NL)
            with open(os.path.join(tmp, "root.c"), "w") as f:
                f.write("int y;" + NL)

            orig_cwd = os.getcwd()
            try:
                os.chdir(tmp)
                ok_res = subprocess.CompletedProcess(
                    args=[], returncode=0, stdout="", stderr=""
                )
                fail_res = subprocess.CompletedProcess(
                    args=[], returncode=1, stdout="", stderr="c89 error"
                )

                with mock.patch("subprocess.run", return_value=ok_res):
                    self.assertTrue(mod.check_file(src_c))
                    self.assertTrue(mod.check_file(root_c))

                with mock.patch("subprocess.run", return_value=fail_res):
                    self.assertFalse(mod.check_file(src_c))

                with mock.patch("subprocess.run", side_effect=OSError("no gcc")):
                    self.assertFalse(mod.check_file(src_c))

                with mock.patch("subprocess.run", return_value=ok_res):
                    with mock.patch.object(
                        sys,
                        "argv",
                        [
                            abs_path,
                            "hdr.h",
                            os.path.join("mod_a", "tests", "t.c"),
                            "test_skip.c",
                            src_c,
                        ],
                    ):
                        runpy.run_path(abs_path, run_name="__main__")

                with mock.patch("subprocess.run", return_value=fail_res):
                    with mock.patch.object(sys, "argv", [abs_path, src_c]):
                        with self.assertRaises(SystemExit) as ctx:
                            runpy.run_path(abs_path, run_name="__main__")
                        self.assertEqual(ctx.exception.code, 1)
            finally:
                os.chdir(orig_cwd)

    def test_check_c99_comments(self):
        mod, abs_path = _load_module(
            "check_c99_comments_mod", "scripts/check_c99_comments.py"
        )

        with tempfile.TemporaryDirectory() as tmp:
            missing = os.path.join(tmp, "missing.c")
            self.assertFalse(mod.check_file(missing))

            valid = os.path.join(tmp, "valid.c")
            with open(valid, "w", encoding="utf-8") as f:
                f.write(
                    NL.join(
                        [
                            "/* block // ok */",
                            f"const char *s = {DQ}http://example.com {BS}{DQ}escaped{BS}{DQ}{DQ};",
                            f"char c1 = {SQ}a{SQ};",
                            f"char c2 = {SQ}{BS}{SQ}{SQ};",
                            "",
                        ]
                    )
                )
            self.assertTrue(mod.check_file(valid))

            invalid = os.path.join(tmp, "invalid.c")
            with open(invalid, "w", encoding="utf-8") as f:
                f.write("int a = 1; // bad comment" + NL)
            self.assertFalse(mod.check_file(invalid))

            with mock.patch.object(sys, "argv", [abs_path, valid]):
                runpy.run_path(abs_path, run_name="__main__")

            with mock.patch.object(sys, "argv", [abs_path, invalid]):
                with self.assertRaises(SystemExit) as ctx:
                    runpy.run_path(abs_path, run_name="__main__")
                self.assertEqual(ctx.exception.code, 1)

    def test_check_guards(self):
        mod, abs_path = _load_module("check_guards_mod", "scripts/check_guards.py")

        with tempfile.TemporaryDirectory() as tmp:
            self.assertFalse(mod.check_file(os.path.join(tmp, "missing.h")))

            good_h = os.path.join(tmp, "good.h")
            with open(good_h, "w", encoding="utf-8") as f:
                f.write(
                    NL.join(
                        [
                            "/* clang-format off */",
                            "#include <stddef.h>",
                            "/* clang-format on */",
                            "#ifdef __cplusplus",
                            f"extern {DQ}C{DQ} {{",
                            "#endif",
                            "int foo(void);",
                            "#ifdef __cplusplus",
                            "}",
                            "#endif",
                            "",
                        ]
                    )
                )
            self.assertTrue(mod.check_file(good_h))

            bad_h = os.path.join(tmp, "bad.h")
            with open(bad_h, "w", encoding="utf-8") as f:
                f.write(
                    NL.join(
                        [
                            "/* clang-format off */",
                            "/* clang-format on */",
                            "/* clang-format off */",
                            "",
                        ]
                    )
                )
            self.assertFalse(mod.check_file(bad_h))

            good_c = os.path.join(tmp, "good.c")
            with open(good_c, "w", encoding="utf-8") as f:
                f.write(
                    NL.join(
                        [
                            "/* clang-format off */",
                            "/* clang-format on */",
                            "int x;",
                            "",
                        ]
                    )
                )
            self.assertTrue(mod.check_file(good_c))

            with mock.patch.object(sys, "argv", [abs_path, good_h, good_c]):
                runpy.run_path(abs_path, run_name="__main__")

            with mock.patch.object(sys, "argv", [abs_path, bad_h]):
                with self.assertRaises(SystemExit) as ctx:
                    runpy.run_path(abs_path, run_name="__main__")
                self.assertEqual(ctx.exception.code, 1)

    def test_check_patches(self):
        mod, abs_path = _load_module("check_patches_mod", "scripts/check_patches.py")

        with tempfile.TemporaryDirectory() as tmp:
            os.makedirs(os.path.join(tmp, ".git"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "build"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "scripts"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "mod1"), exist_ok=True)

            tracked_p = os.path.join(tmp, "mod1", "tracked.patch")
            untracked_p = os.path.join(tmp, "mod1", "untracked.patch")
            with open(tracked_p, "w", encoding="utf-8") as f:
                f.write("--- a" + NL + "+++ b" + NL)
            with open(untracked_p, "w", encoding="utf-8") as f:
                f.write("--- a" + NL + "+++ b" + NL)
            with open(
                os.path.join(tmp, "mod1", "notes.txt"), "w", encoding="utf-8"
            ) as f:
                f.write("ignore" + NL)

            cmake_path = os.path.join(tmp, "mod1", "CMakeLists.txt")
            with open(cmake_path, "w", encoding="utf-8") as f:
                f.write(
                    NL.join(
                        [
                            "PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/tracked.patch",
                            "PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/untracked.patch",
                            "PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/missing.patch",
                            "",
                        ]
                    )
                )

            fake_script = os.path.join(tmp, "scripts", "check_patches.py")

            def fake_git_status(cmd, **kwargs):
                target = cmd[-1]
                out = (
                    "?? untracked.patch" + NL
                    if target.endswith("untracked.patch")
                    else ""
                )
                return subprocess.CompletedProcess(
                    args=cmd, returncode=0, stdout=out, stderr=""
                )

            with mock.patch.object(mod, "__file__", fake_script):
                with mock.patch("subprocess.run", side_effect=fake_git_status):
                    with self.assertRaises(SystemExit) as ctx:
                        mod.main()
                    self.assertEqual(ctx.exception.code, 1)

            with open(cmake_path, "w", encoding="utf-8") as f:
                f.write(
                    "PATCH_COMMAND git apply ${CMAKE_CURRENT_SOURCE_DIR}/tracked.patch"
                    + NL
                )
            with open(abs_path, "r", encoding="utf-8") as src_f:
                code_text = src_f.read()

            with mock.patch("os.path.abspath", return_value=fake_script):
                with mock.patch("subprocess.run", side_effect=fake_git_status):
                    with self.assertRaises(SystemExit) as ctx2:
                        exec(
                            compile(code_text, abs_path, "exec"),
                            {"__name__": "__main__", "__file__": fake_script},
                        )
                    self.assertEqual(ctx2.exception.code, 0)

    def test_scrape_manpages(self):
        mod, abs_path = _load_module(
            "scrape_manpages_mod", "scripts/scrape_manpages.py"
        )

        self.assertEqual(mod.strip_html("<b>int   foo</b>"), "int foo")

        with mock.patch("urllib.request.urlopen", side_effect=OSError("offline")):
            self.assertIsNone(mod.scrape_function("foo", "foo.h"))

        html_ok = NL.join(
            [
                f"<a id={DQ}SYNOPSIS{DQ}></a><h2>SYNOPSIS</h2>",
                "<pre>#include &lt;foo.h&gt;",
                "int <b>foo</b>(int a,",
                "            const char *b,",
                "            int c);",
                "int other(void);</pre>",
                f"<a id={DQ}DESCRIPTION{DQ}></a><h2>DESCRIPTION</h2>",
                "<pre><b>foo</b>() does",
                "something useful.",
                "",
                "Second para.</pre>",
            ]
        )
        html_single = NL.join(
            [
                f"<a id={DQ}SYNOPSIS{DQ}></a><h2>SYNOPSIS</h2>",
                "<pre>int bar(void);</pre>",
                f"<a id={DQ}DESCRIPTION{DQ}></a><h2>DESCRIPTION</h2>",
                "<pre>bar desc.</pre>",
            ]
        )
        html_incomplete = NL.join(
            [
                f"<a id={DQ}SYNOPSIS{DQ}></a><h2>SYNOPSIS</h2>",
                "<pre>int baz(",
                "int x</pre>",
            ]
        )
        html_none = "<html><body>No sections</body></html>"

        calls = {"n": 0}

        def fake_urlopen(url):
            calls["n"] += 1
            if calls["n"] == 1:
                raise OSError("404")
            return io.BytesIO(html_ok.encode("utf-8"))

        with mock.patch("urllib.request.urlopen", side_effect=fake_urlopen):
            res = mod.scrape_function("foo", "sys/foo.h")
            self.assertIsNotNone(res)
            self.assertIn("foo(", res["prototype"])

        with mock.patch(
            "urllib.request.urlopen",
            return_value=io.BytesIO(html_single.encode("utf-8")),
        ):
            res2 = mod.scrape_function("bar", "bar.h")
            self.assertIsNotNone(res2)

        with mock.patch(
            "urllib.request.urlopen",
            return_value=io.BytesIO(html_incomplete.encode("utf-8")),
        ):
            self.assertIsNone(mod.scrape_function("baz", "baz.h"))

        with mock.patch(
            "urllib.request.urlopen",
            return_value=io.BytesIO(html_none.encode("utf-8")),
        ):
            self.assertIsNone(mod.scrape_function("qux", "qux.h"))

        with tempfile.TemporaryDirectory() as tmp:
            orig_cwd = os.getcwd()
            try:
                os.chdir(tmp)
                with mock.patch.object(sys, "argv", [abs_path]):
                    with self.assertRaises(SystemExit) as ctx:
                        mod.main()
                    self.assertEqual(ctx.exception.code, 1)

                def urlopen_seq(url):
                    if "bar." in url:
                        return io.BytesIO(html_single.encode("utf-8"))
                    raise OSError("missing")

                with mock.patch("urllib.request.urlopen", side_effect=urlopen_seq):
                    with mock.patch.object(
                        sys, "argv", [abs_path, "sys/bar.h", "bar", "missing_fn"]
                    ):
                        mod.main()
                    with mock.patch.object(sys, "argv", [abs_path, "sys/bar.h", "bar"]):
                        runpy.run_path(abs_path, run_name="__main__")
            finally:
                os.chdir(orig_cwd)

    def test_transform_crt(self):
        mod, abs_path = _load_module("transform_crt_mod", "scripts/transform_crt.py")

        with tempfile.TemporaryDirectory() as tmp:
            missing = os.path.join(tmp, "nope.c")
            self.assertFalse(mod.process_file(missing))

            sample = os.path.join(tmp, "sample.c")
            with open(sample, "w", encoding="utf-8") as f:
                f.write("int x = 0;" + NL)

            self.assertFalse(mod.process_file(sample))

            with mock.patch.object(
                mod, "transform_crt_content", return_value="int x = 1;" + NL
            ):
                self.assertTrue(mod.process_file(sample))

            with mock.patch.object(sys, "argv", [abs_path, sample]):
                runpy.run_path(abs_path, run_name="__main__")

    def test_update_shields(self):
        mod, abs_path = _load_module("update_shields_mod", "scripts/update_shields.py")

        with tempfile.TemporaryDirectory() as tmp:
            orig_cwd = os.getcwd()
            try:
                os.chdir(tmp)
                mod.update_readme()

                for d in [
                    "build/include",
                    "include",
                    "posix-fcntl/include",
                    "posix-fcntl/tests",
                    "posix-semaphore/include",
                    "posix-mqueue/include",
                    "posix-libproc/include",
                    "posix-libproc/tests",
                    "posix-arpa/include",
                    "posix-arpa/tests",
                    "custom-mod/include",
                    "custom-mod/tests",
                    "empty-mod/include",
                    "posix-pthread/tests",
                    "posix-ipc/tests",
                ]:
                    os.makedirs(os.path.join(tmp, d), exist_ok=True)

                with open(os.path.join(tmp, "build/include/ign.h"), "w") as f:
                    f.write("int ignored_fn(void);" + NL)
                with open(os.path.join(tmp, "include/root_ign.h"), "w") as f:
                    f.write("int root_ignored(void);" + NL)
                with open(os.path.join(tmp, "empty-mod/include/empty.h"), "w") as f:
                    f.write("/* no functions */" + NL)

                with open(os.path.join(tmp, "posix-fcntl/include/fcntl.h"), "w") as f:
                    f.write(
                        NL.join(
                            [
                                "return ignored_ret(x);",
                                "int return (void);",
                                "int int(void);",
                                "int split/*c*/name(int a);",
                                "/** @brief Doc for posix_open */",
                                "int posix_open(const char *p);",
                                "int posix_open(const char *p);",
                                "",
                            ]
                        )
                    )
                with open(os.path.join(tmp, "posix-fcntl/tests/t.c"), "w") as f:
                    f.write("void test_open(void) {}" + NL)

                with open(os.path.join(tmp, "posix-semaphore/include/sem.h"), "w") as f:
                    f.write(
                        NL.join(
                            [
                                "int /*mid*/ sem_nodoc(int *s);",
                                "/** @brief Sem wait */",
                                "int /*mid*/ sem_wait(int *s);",
                                "int prev_stmt;",
                                "int /*mid*/ sem_post(int *s);",
                                "",
                            ]
                        )
                    )
                with open(os.path.join(tmp, "posix-pthread/tests/t.c"), "w") as f:
                    f.write("void test_stub(void) {}" + NL)

                with open(os.path.join(tmp, "posix-mqueue/include/a_mq.h"), "w") as f:
                    f.write(
                        NL.join(
                            [
                                "#define REF_MQ mq_open_x",
                                "int prev;",
                                "/** @brief Second-occurrence doc */",
                                "int /*x*/ mq_open_x(int a);",
                                "int /*x*/ mq_undoc_two(int a);",
                                "",
                            ]
                        )
                    )
                with open(os.path.join(tmp, "posix-mqueue/include/b_mq.h"), "w") as f:
                    f.write("int /*x*/ mq_undoc_two(int a);" + NL)
                with open(os.path.join(tmp, "posix-ipc/tests/t.c"), "w") as f:
                    f.write("void test_ipc(void) {}" + NL)

                with open(os.path.join(tmp, "posix-libproc/include/proc.h"), "w") as f:
                    f.write("int proc_info(int pid);" + NL)
                with open(os.path.join(tmp, "posix-libproc/tests/t.c"), "w") as f:
                    f.write("void f(void) { SKIP(); }" + NL)

                with open(os.path.join(tmp, "posix-arpa/include/inet.h"), "w") as f:
                    f.write(
                        "/** @brief Doc */"
                        + NL
                        + "int inet_aton_x(const char *cp);"
                        + NL
                    )
                with open(os.path.join(tmp, "posix-arpa/tests/t.c"), "w") as f:
                    f.write("void test_arpa(void) {}" + NL)

                with open(os.path.join(tmp, "custom-mod/include/cust.h"), "w") as f:
                    f.write("/** old doc */;" + NL + "int cust_untested(int a);" + NL)
                with open(os.path.join(tmp, "custom-mod/tests/t.c"), "w") as f:
                    f.write("void nothing(void) {}" + NL)

                with open(os.path.join(tmp, "README.md"), "w") as f:
                    f.write("plain text without heading" + NL)
                mod.update_readme()

                with open(os.path.join(tmp, "README.md"), "w") as f:
                    f.write(
                        NL.join(
                            [
                                "# Project Title",
                                "![Doc Coverage](https://img.shields.io/badge/doc__coverage-10%25-red)",
                                "![Test Coverage](https://img.shields.io/badge/test__coverage-10%25-red)",
                                "",
                                "",
                                "",
                                "Body",
                                "",
                            ]
                        )
                    )
                runpy.run_path(abs_path, run_name="__main__")
                with open(os.path.join(tmp, "README.md"), "r") as f:
                    updated = f.read()
                self.assertIn("doc__coverage", updated)
                self.assertIn("test__coverage", updated)
            finally:
                os.chdir(orig_cwd)

    def test_verify_api_contract(self):
        mod, abs_path = _load_module(
            "verify_api_contract_mod", "scripts/verify_api_contract.py"
        )

        with tempfile.TemporaryDirectory() as tmp:
            os.makedirs(os.path.join(tmp, "mod", "include", "sys"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "mod", "src"), exist_ok=True)
            os.makedirs(os.path.join(tmp, "specs"), exist_ok=True)

            with open(os.path.join(tmp, "mod", "include", "sys", "wait.h"), "w") as f:
                f.write("int waitpid(int pid, int *st, int opt);" + NL)
            with open(os.path.join(tmp, "mod", "include", "plain.h"), "w") as f:
                f.write("int plain_fn(void);" + NL)

            self.assertIsNotNone(mod.find_header(tmp, "sys/wait.h"))
            self.assertIsNotNone(mod.find_header(tmp, "plain.h"))
            self.assertIsNone(mod.find_header(tmp, "nonexistent.h"))

            bad_json = os.path.join("specs", "bad.json")
            with open(os.path.join(tmp, bad_json), "w") as f:
                f.write("{broken json")

            ok_spec = os.path.join("specs", "ok.json")
            with open(os.path.join(tmp, ok_spec), "w") as f:
                json.dump(
                    {
                        "sys/wait.h": {"waitpid": {}},
                        "plain.h": {"plain_fn": {}},
                    },
                    f,
                )

            fail_spec = os.path.join("specs", "fail.json")
            with open(os.path.join(tmp, fail_spec), "w") as f:
                json.dump(
                    {
                        "missing.h": {"foo": {}},
                        "plain.h": {"missing_sym": {}},
                    },
                    f,
                )

            orig_cwd = os.getcwd()
            try:
                os.chdir(tmp)
                self.assertFalse(mod.verify_contract(tmp, bad_json))

                with mock.patch.object(sys, "argv", [abs_path, "ignored.txt", ok_spec]):
                    runpy.run_path(abs_path, run_name="__main__")

                with mock.patch.object(sys, "argv", [abs_path, fail_spec]):
                    with self.assertRaises(SystemExit) as ctx:
                        runpy.run_path(abs_path, run_name="__main__")
                    self.assertEqual(ctx.exception.code, 1)
            finally:
                os.chdir(orig_cwd)


if __name__ == "__main__":
    unittest.main()
