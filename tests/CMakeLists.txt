include_directories(${libsysinfo_SOURCE_DIR}/src)

file(GLOB TESTS *.cpp)

foreach (test_file ${TESTS})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} sysinfo)
endforeach (test_file)
