#!/bin/bash
# Check for C99 comments (//) in modified .c and .h files
for file in "$@"; do
    if grep -q -E "//" "$file"; then
        echo "Error: C99 style comment (//) found in $file"
        exit 1
    fi
done
exit 0
