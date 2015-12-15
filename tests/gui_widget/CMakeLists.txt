include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt5Widgets)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_executable(test_gui_widgets test_gui_widgets.cpp mainwindow.cpp mainwindow.ui)
target_link_libraries(test_gui_widgets sysinfo Qt5::Widgets)
