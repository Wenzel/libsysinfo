# - Try to find PROCPS
# Once done this will define
#  PROCPS_FOUND - System has PROCPS
#  PROCPS_INCLUDE_DIRS - The PROCPS include directories
#  PROCPS_LIBRARIES - The libraries needed to use PROCPS
#  PROCPS_DEFINITIONS - Compiler switches required for using PROCPS

include (FindPackageHandleStandardArgs)

find_path(PROCPS_INCLUDE_DIR proc/procps.h)

find_library(PROCPS_LIBRARY NAMES procps)

find_package_handle_standard_args (procps DEFAULT_MSG PROCPS_LIBRARY PROCPS_INCLUDE_DIR)
