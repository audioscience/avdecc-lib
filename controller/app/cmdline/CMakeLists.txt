cmake_minimum_required (VERSION 2.8) 
project (controller)
enable_testing()

include_directories( src ../../lib/include ../../../jdksavdecc-c/include )

file(GLOB_RECURSE CMDLINE_INCLUDES "src/*.h" )

file(GLOB_RECURSE CMDLINE_SRC "src/*.cpp" )

add_executable (avdecccmdline ${CMDLINE_SRC} ${CMDLINE_INCLUDES})

target_link_libraries(avdecccmdline controller)

target_link_libraries(avdecccmdline pcap)

