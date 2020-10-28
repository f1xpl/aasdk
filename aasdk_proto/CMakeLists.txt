include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

file(GLOB_RECURSE proto_files ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)

protobuf_generate_cpp(proto_sources proto_headers ${proto_files})

add_library(aasdk_proto SHARED ${proto_headers} ${proto_sources})

target_link_libraries(aasdk_proto ${PROTOBUF_LIBRARIES})

set_target_properties(aasdk_proto PROPERTIES PUBLIC_HEADER "${proto_headers}")

install(TARGETS aasdk_proto
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/aasdk_proto)
