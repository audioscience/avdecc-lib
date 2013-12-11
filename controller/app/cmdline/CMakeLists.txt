cmake_minimum_required (VERSION 2.8) 
project (controller)
enable_testing()

if(APPLE)
  include_directories( include src src/osx ../../jdksavdecc-c/include )
  set(PCAP_NAME pcap)
elseif(UNIX)
  include_directories( include src src/linux ../../jdksavdecc-c/include )
  set(PCAP_NAME pcap)
elseif(WIN32)
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    link_directories($ENV{WPCAP_DIR}/Lib/x64)
  elseif( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    link_directories($ENV{WPCAP_DIR}/Lib)
  endif()
  include_directories( src ../../lib/include ../../../jdksavdecc-c/include $ENV{WPCAP_DIR}/Include )
  set(PCAP_NAME wpcap)
endif()

file(GLOB_RECURSE CMDLINE_INCLUDES "src/*.h" )

file(GLOB_RECURSE CMDLINE_SRC "src/*.cpp" )

add_executable (avdecccmdline ${CMDLINE_SRC} ${CMDLINE_INCLUDES})

target_link_libraries(avdecccmdline controller)

target_link_libraries(avdecccmdline ${PCAP_NAME})
