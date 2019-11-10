#!/bin/bash
 
mkdir -p artifacts >/dev/null
pushd . >/dev/null
cd artifacts >/dev/null
cmake $* ..
popd >/dev/null
