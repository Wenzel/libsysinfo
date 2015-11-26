cmake_minimum_required(VERSION 2.8)

# set cmake module path, to search in cmake/modules first
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/find")
#-----------------------------------------------------------------------------
#                               MODULES
#-----------------------------------------------------------------------------
include(static_analysis)
include(doxygen)
include(ccache)
#-----------------------------------------------------------------------------
#                           GENERAL CONFIGURATION
#-----------------------------------------------------------------------------
project(sysinfo CXX)
#-----------------------------------------------------------------------------
#                              DEPENDENCIES
#-----------------------------------------------------------------------------
set(ALL_INCLUDES "")
set(ALL_LIBS "")


#-----------------------------------------------------------------------------
#                           BUILD TYPES & FLAGS
#-----------------------------------------------------------------------------
include(sanitizers)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++14 -fdiagnostics-color")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror -O2")

# strip binary in release mode
if(CMAKE_BUILD_TYPE MATCHES "RELEASE")
    set(CMAKE_EXE_LINKER_FLAGS "-s")
endif()
#-----------------------------------------------------------------------------
#                               SOURCES
#-----------------------------------------------------------------------------
add_subdirectory(src)
include(uninstall)

#-----------------------------------------------------------------------------
#                               TESTS
#-----------------------------------------------------------------------------
add_subdirectory(tests)

# add_subdirectory("${CMAKE_SOURCE_DIR}/cmake/cpack") # enable packaging with CPack
#-----------------------------------------------------------------------------
#                               BUILD SUMMARY
#-----------------------------------------------------------------------------
message(STATUS " ↳ Compiler     → ${CMAKE_CXX_COMPILER}")
message(STATUS " ↳ Prefix       → ${CMAKE_INSTALL_PREFIX}")
message(STATUS " ↳ Build        → ${CMAKE_BUILD_TYPE}")
message(STATUS " ↳ Sanitizers   → ${COMPILER_SANITIZER}")
message(STATUS " ↳ CXX Flags    → ${CMAKE_CXX_FLAGS}")
