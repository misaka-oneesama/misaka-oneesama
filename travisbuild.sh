#!/usr/bin/env bash

### DON'T CHANGE OR REMOVE THIS LINE ####################################
cd "$(dirname "$(realpath "$0")")"
#########################################################################

# note:
#   when modifying this script please note that we are inside a
#   ArchLinux chroot

### Setup environment

export QMAKE_BIN="qmake-qt5"

# Clone submodules
git submodule update --init

# Create and enter build directory
mkdir build && cd build

#########################################################################

# Build
"$QMAKE_BIN" ../misaka-oneesama.pro
make -j4
