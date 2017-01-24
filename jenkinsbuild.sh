#!/usr/bin/env bash

### DON'T CHANGE OR REMOVE THIS LINE ####################################
cd "$(dirname "$(realpath "$0")")"
#########################################################################

# note to self: GCC compiled binary segfaults on FreeBSD

### Setup environment

export QMAKE_BIN="/usr/local/lib/qt5/bin/qmake"
export QMAKESPEC="/usr/local/lib/qt5/mkspecs/freebsd-clang"

# Make sure that there are no old builds
[ -d build ] && rm -rf build

# Clone submodules
git submodule update --init

# Create and enter build directory
mkdir build && cd build

#########################################################################

# Build
"$QMAKE_BIN" ../misaka-oneesama.pro
make -j4
