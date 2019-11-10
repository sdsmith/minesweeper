#!/bin/bash

pushd . >/dev/null
cd artifacts

if ! [[ -f compile_commands.json ]]; then
    echo "No compilation database:"
    echo "    Run config.sh with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON and do a full build."
    exit 0
fi

find ../src -regex '.*\.\(cpp\)' -exec clang-tidy --header-filter=../src $* {} \;
popd >/dev/null
