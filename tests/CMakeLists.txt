include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(test_sysinfo main.cpp)

target_link_libraries(test_sysinfo sysinfo)
