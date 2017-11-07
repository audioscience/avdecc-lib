cmake_minimum_required (VERSION 2.8) 
project (avdecc-lib_controller)
enable_testing()

include_directories( ../../../lib/include )

if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories($ENV{WPCAP_DIR}/Lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories($ENV{WPCAP_DIR}/Lib)
  endif()
endif()  

add_executable (test_stream_formats "avdecc_stream_formats_main.cpp")
if(WIN32)
  target_link_libraries(test_stream_formats wpcap)
endif()
target_link_libraries(test_stream_formats avdecc-lib_controller)
