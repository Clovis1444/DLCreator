#!/bin/bash
# This script builds project

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
cd "$SCRIPT_DIR" || exit 1

# If the first argument is configure
if [ "$1" = "-c" ] || [ "$1" = "--configure" ]; then
    # Configure
    cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=clang -DCMAKE_CXX_COMPILER:FILEPATH=clang++ -S${SCRIPT_DIR} -B${SCRIPT_DIR}\\build -G Ninja || exit 1
fi
# Build
cmake --build ${SCRIPT_DIR}/build --config Release --target DLCreator || exit 1