project(protobuf)

find_package(Protobuf REQUIRED 3.15.6)
message(${PROTOBUF_LIBRARIES})

#每次执行cmake重新生成proto
execute_process(COMMAND ${Protobuf_PROTOC_EXECUTABLE} -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} tetris.proto)

file(GLOB PROTO_SRCS *.cc)
show_files("${PROTO_SRCS}" "PROTO_SRCS")

add_library(${PROJECT_NAME} STATIC ${PROTO_SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC ${Protobuf_INCLUDE_DIRS} .)

target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})