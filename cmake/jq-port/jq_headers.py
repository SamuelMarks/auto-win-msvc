#!/usr/bin/env python3
"""Generate version.h, config.h, and builtin.inc for jq CMake builds."""

import os
import sys


def main() -> None:
    """Entry point for generating jq build headers."""
    builtin_jq = sys.argv[1]
    gen_dir = sys.argv[2]
    os.makedirs(gen_dir, exist_ok=True)

    bs = chr(92)
    dq = chr(34)
    nl = chr(10)
    cr = chr(13)

    os.makedirs(os.path.join(gen_dir, "src"), exist_ok=True)
    for ver_rel in ("version.h", os.path.join("src", "version.h")):
        with open(os.path.join(gen_dir, ver_rel), "w", encoding="utf-8") as vf:
            vf.write(f"#define JQ_VERSION {dq}1.8.2{dq}{nl}")

    for cfg_rel in ("config_opts.inc", os.path.join("src", "config_opts.inc")):
        with open(os.path.join(gen_dir, cfg_rel), "w", encoding="utf-8") as of:
            of.write(f"#define JQ_CONFIG {dq}--with-oniguruma=builtin{dq}{nl}")

    with open(os.path.join(gen_dir, "config.h"), "w", encoding="utf-8") as cf:
        cf.write(
            nl.join(
                [
                    "/* Generated config.h for jq via auto-win-msvc */",
                    "#ifndef JQ_AUTO_WIN_MSVC_CONFIG_H",
                    "#define JQ_AUTO_WIN_MSVC_CONFIG_H",
                    f"#define JQ_VERSION {dq}1.8.2{dq}",
                    "#define HAVE_MEMMEM 0",
                    "#define HAVE_STRPTIME 1",
                    "#define HAVE_GMTIME_R 1",
                    "#define HAVE_LOCALTIME_R 1",
                    "#define HAVE_GETTIMEOFDAY 1",
                    "#define HAVE_ISATTY 1",
                    "#define HAVE_SETENV 1",
                    "#define HAVE_PTHREAD_KEY_CREATE 1",
                    "#define HAVE_PTHREAD_ONCE 1",
                    "#define HAVE_LIBONIG 1",
                    "#define HAVE_ONIGURUMA 1",
                    "#define USE_DECNUM 1",
                    "#define IEEE_8087 1",
                    "#define ONIG_STATIC 1",
                    "#endif",
                    "",
                ]
            )
        )

    os.makedirs(os.path.join(gen_dir, "src"), exist_ok=True)
    with open(builtin_jq, "rb") as src_f:
        raw_bytes = src_f.read()

    hex_chunks = []
    for i in range(0, len(raw_bytes), 16):
        chunk = raw_bytes[i : i + 16]
        hex_chunks.append(", ".join(f"0x{b:02x}" for b in chunk) + ",")
    inc_text = nl.join(hex_chunks) + nl

    for out_rel in ("builtin.inc", os.path.join("src", "builtin.inc")):
        with open(os.path.join(gen_dir, out_rel), "w", encoding="utf-8") as inc_f:
            inc_f.write(inc_text)


if __name__ == "__main__":
    main()
