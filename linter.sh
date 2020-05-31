#!/bin/bash

pushd . >/dev/null
cd artifacts

# Clang format
#
echo "Running clang-format..."
find ../src -regex '.*\.\(cpp\|h\)' -exec clang-format -style=file -i {} \;

# Clang-tidy
#
echo "Running clang-tidy..."
if ! [[ -f compile_commands.json ]]; then
    echo "No compilation database:"
    echo "    Run config.sh with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON and do a full build."
    exit 0
fi

find ../src -regex '.*\.\(cpp\|h\)' -exec clang-tidy --header-filter=../src $* {} \;
popd >/dev/null
