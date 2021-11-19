project(protobuf)

find_package(Protobuf REQUIRED 3.15.6)
message(${PROTOBUF_LIBRARIES})

#每当修改tetris.proto时重新生成pb文件
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/tetris.pb.cc ${CMAKE_CURRENT_SOURCE_DIR}/tetris.pb.h
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} tetris.proto
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tetris.proto
        VERBATIM
        )
file(GLOB PROTO_SRCS *.cc)
show_files("${PROTO_SRCS}" "PROTO_SRCS")

add_library(${PROJECT_NAME} STATIC ${PROTO_SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC ${Protobuf_INCLUDE_DIRS} .)

target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})