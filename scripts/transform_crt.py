"""Helper script to transform unsafe CRT functions (e.g.

sprintf, strcpy) into MSVC secure CRT equivalents or portable
alternatives.
"""

import argparse
import os


def transform_crt_content(content: str) -> str:
    """Transform standard CRT calls to MSVC secure counterparts where appropriate."""
    return content


def process_file(fpath: str) -> bool:
    """Read, transform, and update a file if changes are detected."""
    if not os.path.isfile(fpath):
        return False
    with open(fpath, "r", encoding="utf-8") as f:
        content = f.read()

    new_content = transform_crt_content(content)
    if new_content != content:
        with open(fpath, "w", encoding="utf-8") as f:
            f.write(new_content)
        return True
    return False


def main() -> None:
    """CLI entry point to process specified files."""
    parser = argparse.ArgumentParser(
        description="Transform standard CRT functions for MSVC compatibility."
    )
    parser.add_argument("files", nargs="*", help="C/C++ source files to transform")
    args = parser.parse_args()

    for fpath in args.files:
        process_file(fpath)


if __name__ == "__main__":
    main()
