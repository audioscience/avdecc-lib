cmake_minimum_required (VERSION 2.8) 
project (controller)
enable_testing()

if(APPLE)
  include_directories( include src src/osx ../../jdksavdecc-c/include )
elseif(UNIX)
  include_directories( include src src/linux ../../jdksavdecc-c/include )
elseif(WIN32)
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      link_directories($ENV{WPCAP_DIR}/Lib/x64)
	elseif( CMAKE_SIZEOF_VOID_P EQUAL 4 )
      link_directories($ENV{WPCAP_DIR}/Lib)
	endif()
	
  add_definitions(-DAVDECC_CONTROLLER_LIB32_EXPORTS -D_CRT_SECURE_NO_WARNINGS)
  include_directories( include src src/msvc ../../jdksavdecc-c/include $ENV{WPCAP_DIR}/Include )
endif()

file(GLOB CONTROLLERLIB_INCLUDES "include/*.h" "src/*.h" )

file(GLOB CONTROLLERLIB_SRC "src/*.cpp" "../../jdksavdecc-c/src/jdksavdecc_pdu.c" "../../jdksavdecc-c/src/jdksavdecc_frame.c")

if(APPLE)
  file(GLOB_RECURSE SYSTEM_SPEC_SRC "src/osx/*.cpp")
elseif(UNIX)
  file(GLOB_RECURSE SYSTEM_SPEC_SRC "src/linux/*.cpp")
elseif(WIN32)
  file(GLOB_RECURSE SYSTEM_SPEC_SRC "src/msvc/*.cpp")
endif()

add_library(controller SHARED ${CONTROLLERLIB_INCLUDES} ${CONTROLLERLIB_SRC} ${SYSTEM_SPEC_SRC})

if(APPLE)
  target_link_libraries(controller pcap)
elseif(UNIX)
  target_link_libraries(controller pcap)
elseif(WIN32)
  target_link_libraries(controller wpcap Iphlpapi Ws2_32)
endif()

