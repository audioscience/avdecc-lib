cmake_minimum_required (VERSION 2.8) 
project (controller)
enable_testing()

include_directories( src ../../lib/include )

if(APPLE)
  set(PCAP_NAME pcap)
elseif(UNIX)
  set(PCAP_NAME pcap)
elseif(WIN32)
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    link_directories($ENV{WPCAP_DIR}/Lib/x64)
  elseif( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    link_directories($ENV{WPCAP_DIR}/Lib)
  endif()
  
  set(PCAP_NAME wpcap)
endif()

file(GLOB_RECURSE CMDLINE_INCLUDES "src/*.h" )

file(GLOB_RECURSE CMDLINE_SRC "src/*.cpp" )

add_executable (avdecccmdline ${CMDLINE_INCLUDES} ${CMDLINE_SRC})

target_link_libraries(avdecccmdline controller)

target_link_libraries(avdecccmdline ${PCAP_NAME})
