include_directories(${ALL_INCLUDES})

add_library(sysinfo
    STATIC
    sysinfo.cpp
    procconnector.cpp
    processinfo.cpp
    mmap.cpp
)

target_link_libraries(sysinfo ${ALL_LIBS})
