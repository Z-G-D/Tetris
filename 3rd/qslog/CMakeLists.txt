cmake_minimum_required(VERSION 3.11)
project(qslog)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

file(GLOB_RECURSE QSLOG_SRC *.cpp)
show_files("${QSLOG_SRC}" "QSLOG_SRC")

add_library(${PROJECT_NAME} STATIC ${QSLOG_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)