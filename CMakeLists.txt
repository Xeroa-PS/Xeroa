cmake_minimum_required (VERSION 3.8)
project (XeroaPS)

if(MSVC) 
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

add_subdirectory(src)