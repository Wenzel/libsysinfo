include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(test_simple test_simple.cpp)
target_link_libraries(test_simple sysinfo)

add_executable(test_procconnector test_procconnector.cpp)
target_link_libraries(test_procconnector sysinfo)

add_subdirectory(gui_widget)
# add_subdirectory(gui_qml)
