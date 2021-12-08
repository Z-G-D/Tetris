cmake_minimum_required(VERSION 3.12)
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

add_compile_definitions(QS_LOG_LINE_NUMBERS)#打印日志对应代码行号
if(${CMAKE_BUILD_TYPE} STREQUAL Release)
    add_compile_definitions(QS_LOG_SEPARATE_THREAD)#发布版中日志异步线程打印
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Linux")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        message(STATUS "using GCC")
    else()
        message(WARNING "other compiler ${CMAKE_CXX_COMPILER_ID} this CMakelists may not work")
    endif()
    add_compile_options(-fPIE)#linux生成位置无关代码
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")#windows with mingw
        message(STATUS "using GCC")
        #设置3方库文件的路径，如已安装或者已设环境变量可以注释下两行
        set(Protobuf_ROOT D:/protobuf-3.15.6;D:/protobuf-3.15.6/mingw_build)
        set(Qt5_ROOT D:/Qt/5.15.2/mingw81_64)
        #如果乱码请调整下两行
        add_compile_options(-finput-charset=utf-8)#设置源码字符集
        add_compile_options(-fexec-charset=utf-8)#设置执行字符集
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")#windows with msvc
        message(STATUS "using Visual Studio C++")
        add_definitions(-DPROTOBUF_USE_DLLS)
        #设置3方库文件的路径，如已安装或者已设环境变量可以注释下两行
        set(Protobuf_ROOT D:/protobuf-3.15.6;D:/protobuf-3.15.6/msvc_build/Debug;D:/protobuf-3.15.6/msvc_build/Release)
        set(Qt5_ROOT D:/Qt/5.15.2/msvc2019_64)
        #如果乱码请调整下两行
        add_compile_options(-source-charset:utf-8)#设置源码字符集
        add_compile_options(-execution-charset:utf-8)#设置执行字符集
    endif()
else ()
    message(WARNING "other platform :${CMAKE_SYSTEM_NAME} this CMakelists may not work: ")
endif ()

add_subdirectory(3rd/kcp)
add_subdirectory(3rd/qslog)
add_subdirectory(3rd/protobuf)
add_subdirectory(3rd/simplecrypt)

add_subdirectory(server)
add_subdirectory(tetris)