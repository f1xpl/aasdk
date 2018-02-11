include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

file(GLOB_RECURSE proto_files ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
protobuf_generate_cpp(proto_sources proto_headers ${proto_files})
add_library(aasdk_proto ${proto_headers} ${proto_sources})
target_link_libraries(aasdk_proto ${PROTOBUF_LIBRARIES})

