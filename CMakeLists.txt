cmake_minimum_required(VERSION 3.11)
project(Tetris)
function(show_files file_list description)
    list(LENGTH file_list len)
    message("show_files: ${description} files: ${len}")
    FOREACH(file_path ${file_list})
        message(STATUS ${file_path})
    ENDFOREACH()
endfunction()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DQS_LOG_LINE_NUMBERS)#打印日志对应代码行号
if(${CMAKE_BUILD_TYPE} STREQUAL Release)
    add_definitions(-DQS_LOG_SEPARATE_THREAD)#发布版中日志异步线程打印
endif()

if(CMAKE_HOST_UNIX)
    add_compile_options(-fPIE)#linux生成位置无关代码
endif()

add_subdirectory(3rd/kcp)
add_subdirectory(3rd/qslog)
add_subdirectory(3rd/protobuf)
add_subdirectory(3rd/simplecrypt)

add_subdirectory(server)
add_subdirectory(tetris)