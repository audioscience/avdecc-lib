#!/bin/bash
set -ev
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make

cd "controller/app/test/stream_formats"
./test_stream_formats
