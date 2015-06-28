
cmake_minimum_required(VERSION 2.8)

project(chilitags)

set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

add_definitions(-std=c++11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

##########################################
##           Options                   ###
##########################################

if(UNIX)
    option(WITH_PTHREADS                "Multithreading support with pthreads" ON)
else()
    option(WITH_PTHREADS                "Multithreading support with pthreads" OFF)
endif()

##########################################
##           Chilitags lib             ###
##########################################

if(WITH_PTHREADS)
    add_definitions(-DHAS_MULTITHREADING)
endif()

if(${OpenCV_VERSION} VERSION_GREATER 2.9.0)
    add_definitions(-DOPENCV3)
endif()

include_directories(lib/chilitags/include)
add_subdirectory(lib/chilitags/src)

##########################################
##           oscpack                   ###
##########################################

include_directories(lib/oscpack)

# separate versions of NetworkingUtils.cpp and UdpSocket.cpp are provided for Win32 and POSIX
# the IpSystemTypePath selects the correct ones based on the current platform

if(WIN32)
 set(IpSystemTypePath lib/oscpack/ip/win32)
 set(LIBS ${LIBS} Ws2_32 winmm)
else(WIN32)
 set(IpSystemTypePath lib/oscpack/ip/posix)
endif(WIN32)

add_library(oscpack 

lib/oscpack/ip/IpEndpointName.h
lib/oscpack/ip/IpEndpointName.cpp

lib/oscpack/ip/NetworkingUtils.h
${IpSystemTypePath}/NetworkingUtils.cpp

lib/oscpack/ip/UdpSocket.h
${IpSystemTypePath}/UdpSocket.cpp

lib/oscpack/ip/PacketListener.h
lib/oscpack/ip/TimerListener.h

lib/oscpack/osc/OscTypes.h
lib/oscpack/osc/OscTypes.cpp 
lib/oscpack/osc/OscHostEndianness.h
lib/oscpack/osc/OscException.h
lib/oscpack/osc/OscPacketListener.h
lib/oscpack/osc/MessageMappingOscPacketListener.h
lib/oscpack/osc/OscReceivedElements.h
lib/oscpack/osc/OscReceivedElements.cpp
lib/oscpack/osc/OscPrintReceivedElements.h
lib/oscpack/osc/OscPrintReceivedElements.cpp
lib/oscpack/osc/OscOutboundPacketStream.h
lib/oscpack/osc/OscOutboundPacketStream.cpp

)

##########################################
##           Modules                   ###
##########################################

#include("${CMAKE_SOURCE_DIR}/cmake/TargetDoc.cmake" OPTIONAL)

add_subdirectory(app)

