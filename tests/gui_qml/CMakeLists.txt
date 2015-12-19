include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt5 REQUIRED COMPONENTS Widgets Declarative Qml)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(test_gui_qml test_gui_qml.cpp qml.qrc)
target_link_libraries(test_gui_qml sysinfo Qt5::Widgets Qt5::Declarative Qt5::Qml)
