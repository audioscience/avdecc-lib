#!/bin/bash

echo "clang format styling avdecc-lib..."
clang-format -i -style=file controller/lib/include/*.h \
 controller/lib/src/*.h controller/lib/src/*.cpp \
 controller/lib/src/linux/*.h controller/lib/src/linux/*.cpp \
 controller/lib/src/msvc/*.h controller/lib/src/msvc/*.cpp \
 controller/lib/src/osx/*.h controller/lib/src/osx/*.cpp \
 controller/app/cmdline/src/*.h controller/app/cmdline/src/*.cpp
echo "Done"
