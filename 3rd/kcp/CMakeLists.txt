cmake_minimum_required(VERSION 3.11)
project(kcp)

file(GLOB_RECURSE KCP_SRCS *.c)
show_files("${KCP_SRCS}" KCP_SRCS)

add_library(${PROJECT_NAME} STATIC ${KCP_SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC .)