project(simplecrypt)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

file(GLOB_RECURSE SIMPLECRYPT_SRC *.cpp)
show_files("${SIMPLECRYPT_SRC}" "SIMPLECRYPT_SRC")

add_library(${PROJECT_NAME} STATIC ${SIMPLECRYPT_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)