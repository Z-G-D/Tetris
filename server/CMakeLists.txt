project(server)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS  Core Network Sql Xml REQUIRED)

file(GLOB PROJECT_SRC *.cpp *h)
show_files("${PROJECT_SRC}" "PROJECT_SRC")

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt5::Core
        PRIVATE Qt5::Network
        PRIVATE Qt5::Sql
        PRIVATE Qt5::Xml
        PRIVATE kcp
        PRIVATE qslog
        PRIVATE protobuf
        PRIVATE simplecrypt)
