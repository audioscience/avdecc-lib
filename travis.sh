#!/bin/bash
set -ev
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
