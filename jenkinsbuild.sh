#!/usr/bin/env bash

### DON'T CHANGE OR REMOVE THIS LINE ####################################
cd "$(dirname "$(realpath "$0")")"
#########################################################################

### Setup environment

# Make sure that there are no old builds
[ -d build ] && rm -rf build

# Clone submodules
git submodule update --init

# Create and enter build directory
mkdir build && cd build

#########################################################################

# Build
qmake ../misaka-oneesama.pro
make -j4
