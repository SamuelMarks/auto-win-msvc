#!/usr/bin/env python3
"""
Verify that all patch files referenced in CMakeLists.txt across all modules
exist on disk and are tracked in git.
"""

import os
import re
import subprocess
import sys


def main():
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    errors = []
    checked = 0

    patch_re = re.compile(
        r"PATCH_COMMAND\s+.*?\$\{CMAKE_CURRENT_SOURCE_DIR\}/([^\s\"')]+)",
        re.DOTALL,
    )

    for root, dirs, files in os.walk(repo_root):
        if ".git" in dirs:
            dirs.remove(".git")
        if "build" in dirs:
            dirs.remove("build")
        for f in files:
            if f == "CMakeLists.txt":
                cmake_file = os.path.join(root, f)
                with open(cmake_file, "r", encoding="utf-8", errors="ignore") as fh:
                    content = fh.read()
                matches = patch_re.findall(content)
                for rel_patch in matches:
                    checked += 1
                    patch_path = os.path.join(root, rel_patch)
                    if not os.path.isfile(patch_path):
                        errors.append(f"Missing patch file on disk: {patch_path}")
                        continue
                    # Check if tracked or staged in git
                    res = subprocess.run(
                        ["git", "status", "--porcelain", patch_path],
                        cwd=repo_root,
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True,
                    )
                    # If it's untracked (??), flag it
                    if res.stdout.startswith("??"):
                        errors.append(f"Patch file untracked in git: {patch_path}")

    print(f"Checked {checked} patch references.")
    if errors:
        for err in errors:
            print(f"ERROR: {err}", file=sys.stderr)
        sys.exit(1)
    else:
        print("All patch files exist and are properly tracked.")
        sys.exit(0)


if __name__ == "__main__":
    main()
