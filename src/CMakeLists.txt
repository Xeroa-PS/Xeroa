cmake_minimum_required (VERSION 3.8)
project(Xeroa VERSION 0.1 LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set (XEROA_SOURCES
	${CMAKE_SOURCE_DIR}/external/kcp/ikcp.c
	./main.cpp
	./GameServer.cpp
	./HttpServer.cpp
	./HttpSession.cpp
	./Kcp.cpp
	./BufferView.cpp
	./DynamicBuffer.cpp
	./BasePacket.cpp
	./Crypto.cpp
	./HandlePingReq.cpp
	./HandlePlayerTokenReq.cpp
)

add_executable (Xeroa ${XEROA_SOURCES})

target_include_directories (Xeroa PUBLIC
	${Protobuf_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/external/kcp
	.
)

target_link_libraries (Xeroa ${Protobuf_LIBRARIES} Protos)
set_property(TARGET Xeroa PROPERTY CXX_STANDARD 23)