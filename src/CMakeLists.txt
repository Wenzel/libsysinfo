include_directories(${ALL_INCLUDES})

add_library(sysinfo
    sysinfo.cpp
    procconnector.cpp
    processinfo.cpp
)

target_link_libraries(sysinfo ${ALL_LIBS})
