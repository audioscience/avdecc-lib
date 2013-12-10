cmake_minimum_required (VERSION 2.8) 
project (controller)
enable_testing()

include_directories( src include ../../jdksavdecc-c/include )

file(GLOB_RECURSE CONTROLLERLIB_INCLUDES "include/*.h" "src/*.h" )

file(GLOB_RECURSE CONTROLLERLIB_SRC "src/*.cpp" )

if(TRUE)
  file(GLOB_RECURSE CONTROLLERLIB_SRC "src/osx/*.cpp" )
endif(TRUE)

add_library (controller ${CONTROLLERLIB_SRC} ${CONTROLLERLIB_INCLUDES})


