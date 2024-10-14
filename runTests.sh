#!/bin/bash
# This script builds tests and run it

# Args:
# --release - release build(program will be build in debug mode without this argument)

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
cd "$SCRIPT_DIR" || exit 1

BUILD_TYPE=Debug

for arg in "$@"; do
    if [ "$arg" = "--release" ]; then
        BUILD_TYPE=Release
    fi
done

# If the first argument is configure
if [ "$1" = "-c" ] || [ "$1" = "--configure" ]; then
    # Configure
    cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=clang -DCMAKE_CXX_COMPILER:FILEPATH=clang++ -S${SCRIPT_DIR} -B${SCRIPT_DIR}\\build -G Ninja || exit 1
fi
# Build
cmake --build ${SCRIPT_DIR}/build --config ${BUILD_TYPE} --target DLCreatorTests || exit 1
# Run
./build/tests/DLCreatorTests
