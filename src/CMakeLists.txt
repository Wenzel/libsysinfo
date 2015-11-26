include_directories(${ALL_INCLUDES})

add_library(sysinfo sysinfo.cpp)

target_link_libraries(sysinfo ${ALL_LIBS})
