project(tetris)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS  Widgets Network Multimedia Xml REQUIRED)
file(GLOB PROJECT_FILE *.cpp *.h *.ui)
show_files("${PROJECT_FILE}" "PROJECT_FILE")

qt5_add_big_resources(QRC_FILES res.qrc)
add_executable(${PROJECT_NAME} ${PROJECT_FILE} ${QRC_FILES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt5::Widgets
        PRIVATE Qt5::Network
        PRIVATE Qt5::Multimedia
        PRIVATE Qt5::Xml
        PRIVATE kcp
        PRIVATE qslog
        PRIVATE protobuf
        PRIVATE simplecrypt
        )