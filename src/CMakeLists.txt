cmake_minimum_required (VERSION 3.8)
project(Xeroa VERSION 0.1 LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(Boost 1.73.0 COMPONENTS program_options REQUIRED)

set (XEROA_SOURCES
	${CMAKE_SOURCE_DIR}/external/kcp/ikcp.c
	./main.cpp
	./GameServer.cpp
	./HttpServer.cpp
	./HttpSession.cpp
	./Kcp.cpp
	./BufferView.cpp
	./DynamicBuffer.cpp
)

add_executable (Xeroa ${XEROA_SOURCES})

target_include_directories (Xeroa PUBLIC
	${Boost_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/external/kcp
	.
)

target_link_libraries (Xeroa ${CMAKE_DL_LIBS} Boost::program_options)
set_property(TARGET Xeroa PROPERTY CXX_STANDARD 23)