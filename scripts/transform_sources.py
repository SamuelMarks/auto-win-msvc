#!/usr/bin/env python3
"""
Automated Source Transformation Engine for MSVC (auto-win-msvc).
Transforms unsupported C99 constructs (such as Variable Length Arrays)
into strict C89 / MSVC-compatible constructs at build time.
"""

import argparse
import os
import re
import sys


def transform_source_content(content: str) -> str:
    """
    Apply AST-like heuristic regex rewrites to eliminate VLAs and C99 idioms.
    """
    # 1. Transform memcached.c extras_len VLA pattern
    # Original:
    #   size_t extras_len = sizeof(":unix:") + sizeof("65535");
    #   char addr[MAXPATHLEN + extras_len];
    #   char svr_addr[MAXPATHLEN + extras_len];
    memcached_vla_pattern = re.compile(
        r'(\s*)size_t\s+extras_len\s*=\s*sizeof\(":unix:"\)\s*\+\s*sizeof\("65535"\);\s*\n'
        r'\s*char\s+addr\[MAXPATHLEN\s*\+\s*extras_len\];\s*\n'
        r'\s*char\s+svr_addr\[MAXPATHLEN\s*\+\s*extras_len\];',
        re.MULTILINE,
    )
    if memcached_vla_pattern.search(content):
        replacement = (
            "\\1#ifndef MC_EXTRAS_LEN\n"
            '\\1#define MC_EXTRAS_LEN (sizeof(":unix:") + sizeof("65535"))\n'
            "\\1#endif\n"
            "\\1size_t extras_len = MC_EXTRAS_LEN;\n"
            "\\1char addr[MAXPATHLEN + MC_EXTRAS_LEN];\n"
            "\\1char svr_addr[MAXPATHLEN + MC_EXTRAS_LEN];"
        )
        content = memcached_vla_pattern.sub(replacement, content)

    # 2. Transform proto_bin.c mech[nkey+1] VLA pattern
    # Original:
    #   char mech[nkey+1];
    proto_bin_vla = re.compile(r"char\s+mech\[nkey\s*\+\s*1\];")
    content = proto_bin_vla.sub(
        r"char mech[256]; /* bounded by protocol max key length */", content
    )

    # 3. Generic VLA pattern: char name[expr] where expr has non-constant identifier
    has_alloca = False

    def vla_alloca_repl(match):
        nonlocal has_alloca
        indent = match.group(1)
        type_str = match.group(2)
        var_name = match.group(3)
        expr = match.group(4).strip()
        # Avoid matching constant expressions (numbers, sizeof, macros, uppercase expressions)
        if re.match(r"^[0-9A-Z_+\-*/ ()\t]+$", expr) or "sizeof" in expr:
            return match.group(0)
        has_alloca = True
        return (
            f"{indent}{type_str} *{var_name} = "
            f"({type_str} *)_alloca(({expr}) * sizeof({type_str}));"
        )

    # Transform local variable arrays: e.g. "    char temp[len + 1];"
    generic_vla_pattern = re.compile(
        r"^(\s+)((?:unsigned\s+)?(?:char|int|uint8_t|uint16_t|uint32_t|int32_t))\s+([a-zA-Z0-9_]+)\[([^\]\n]+)\];",
        re.MULTILINE,
    )
    content = generic_vla_pattern.sub(vla_alloca_repl, content)

    if has_alloca and "#include <malloc.h>" not in content:
        content = "/* clang-format off */\n#include <malloc.h>\n/* clang-format on */\n" + content

    return content


def transform_file(input_path: str, output_path: str) -> None:
    with open(input_path, "r", encoding="utf-8", errors="ignore") as f:
        original = f.read()

    transformed = transform_source_content(original)

    out_dir = os.path.dirname(os.path.abspath(output_path))
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(transformed)


def main():
    parser = argparse.ArgumentParser(
        description="Transform C source files for MSVC compatibility."
    )
    parser.add_argument("inputs", nargs="+", help="Input file(s) or input/output pair")
    parser.add_argument("--output", "-o", help="Output file path (for single input)")
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
        print("Usage: transform_sources.py <input> <output> OR --out-dir <dir> <files...>")
        sys.exit(1)


if __name__ == "__main__":
    main()
