#!/bin/bash

if ! [[ -d "artifacts" ]]; then
    echo "run config.sh"
    exit 0
fi

pushd . >/dev/null
cd artifacts
make $*
popd >/dev/null
