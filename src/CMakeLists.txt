cmake_minimum_required (VERSION 3.8)
project(Xeroa VERSION 0.1 LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(XEROA_PACKETS
	game/packets/PingReq.cpp
	game/packets/PingReq.h
	game/packets/PlayerLoginReq.cpp
	game/packets/PlayerLoginReq.h
	game/packets/PlayerTokenReq.cpp
	game/packets/PlayerTokenReq.h
)

set(XEROA_SERVERS
	${CMAKE_SOURCE_DIR}/external/kcp/ikcp.c # Probably a bad idea but whatever

	game/GameServer.cpp
	game/GameServer.h
	game/Kcp.cpp
	game/Kcp.h
	http/HttpServer.cpp
	http/HttpServer.h
	http/HttpSession.cpp
	http/HttpSession.h
)

set (XEROA_UTILS
	util/BasePacket.cpp
	util/BasePacket.h
	util/BufferView.cpp
	util/BufferView.h
	util/Crypto.cpp
	util/Crypto.h
	util/DynamicBuffer.cpp
	util/DynamicBuffer.h
	util/PacketIds.h
	util/util.h
)

set (XEROA_MAIN
	main.cpp
	main.h
)

add_executable (Xeroa ${XEROA_PACKETS} ${XEROA_SERVERS} ${XEROA_UTILS} ${XEROA_MAIN})
set_property(TARGET Xeroa PROPERTY CXX_STANDARD 23)

target_include_directories (Xeroa PUBLIC
	${Protobuf_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/external/kcp
	.
)

target_link_libraries (Xeroa ${Protobuf_LIBRARIES} Protos)