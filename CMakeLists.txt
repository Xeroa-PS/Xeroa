cmake_minimum_required (VERSION 3.8)
project (XeroaPS)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.73.0 REQUIRED)

if(MSVC) 
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

if ( Protobuf_FOUND )
    message( STATUS "Protobuf version : ${Protobuf_VERSION}" )
    message( STATUS "Protobuf include path : ${Protobuf_INCLUDE_DIRS}" )
    message( STATUS "Protobuf libraries : ${Protobuf_LIBRARIES}" )
    message( STATUS "Protobuf compiler libraries : ${Protobuf_PROTOC_EXECUTABLE}")
else()
    message( WARNING "Protobuf package not found -> specify search path via PROTOBUF_ROOT variable")
endif()

add_subdirectory(src)
add_subdirectory(proto)