cmake_minimum_required (VERSION 3.8)
project (Protos)

set(PROTO_SRCS "")
set(PROTO_HDRS "")

file(GLOB ProtoFiles "./*.proto")

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

add_library(Protos STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(Protos PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
