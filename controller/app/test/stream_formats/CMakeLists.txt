cmake_minimum_required (VERSION 2.8) 
project (avdecc-lib_controller)
enable_testing()

include_directories( ../../../lib/include )
add_executable (test_stream_formats "avdecc_stream_formats_main.cpp")
target_link_libraries(test_stream_formats avdecc-lib_controller)
