cmake_minimum_required (VERSION 2.8) 
project (controller)
enable_testing()

include_directories( src ../../lib/include ../../jdksavdecc-c/include )

file(GLOB_RECURSE CMDLINE_INCLUDES "src/*.h" )

file(GLOB_RECURSE CMDLINE_SRC "src/*.cpp" )

if(APPLE)
  add_executable (avdecccmdline ${CMDLINE_INCLUDES} ${CMDLINE_SRC})
  set(PCAP_NAME pcap)
  set(READLINE_NAME edit)
elseif(UNIX)
  add_executable (avdecccmdline ${CMDLINE_INCLUDES} ${CMDLINE_SRC})
  set(PCAP_NAME pcap)
  set(READLINE_NAME readline)
  target_link_libraries(avdecccmdline rt)
elseif(WIN32)
  set(PCAP_NAME wpcap)
  include_directories( src src/msvc ../../lib/include ../../jdksavdecc-c/include )
  file(GLOB_RECURSE SYSTEM_SPEC_SRC "src/msvc/*.c")

  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    link_directories($ENV{WPCAP_DIR}/Lib/x64)
  elseif( CMAKE_SIZEOF_VOID_P EQUAL 4 )
    link_directories($ENV{WPCAP_DIR}/Lib)
  endif()

  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_executable (avdecccmdline ${CMDLINE_INCLUDES} ${CMDLINE_SRC} ${SYSTEM_SPEC_SRC})
endif()

target_link_libraries(avdecccmdline controller)

target_link_libraries(avdecccmdline ${PCAP_NAME})

target_link_libraries(avdecccmdline ${READLINE_NAME})
