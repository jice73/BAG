# The name of our project is "openns". CMakeLists files in this project can
# refer to the root source directory of the project as ${OPENNS_SOURCE_DIR} and
# to the root binary directory of the project as ${OPENNS_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)
project (OPENNS)

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
SET (OPENNS_SRC_DIR                             ${OPENNS_SOURCE_DIR}/api)
SET (OPENNS_EXAMPLES_SOURCE_DIR                 ${OPENNS_SOURCE_DIR}/examples)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE     ${OPENNS_SOURCE_DIR}/bin)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG       ${OPENNS_SOURCE_DIR}/bin)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE     ${OPENNS_SOURCE_DIR}/lib)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG       ${OPENNS_SOURCE_DIR}/lib)

#-----------------------------------------------------------------------------
# parse the full version number from bag.h and include in OPENNS_VERS_INFO
#-----------------------------------------------------------------------------
FILE (READ ${OPENNS_SRC_DIR}/bag.h _bag_h_contents)
STRING (REGEX REPLACE ".*#define[ \t]+BAG_VER_MAJOR[ \t]+([0-9]*).*$"
    "\\1" OPENNS_VERS_MAJOR ${_bag_h_contents})
STRING (REGEX REPLACE ".*#define[ \t]+BAG_VER_MINOR[ \t]+([0-9]*).*$"
    "\\1" OPENNS_VERS_MINOR ${_bag_h_contents})
STRING (REGEX REPLACE ".*#define[ \t]+BAG_VER_REVISION[ \t]+([0-9]*).*$"
    "\\1" OPENNS_VERS_RELEASE ${_bag_h_contents})

#-----------------------------------------------------------------------------
SET (OPENNS_PACKAGE "openns")
SET (OPENNS_PACKAGE_NAME "OPENNS")
SET (OPENNS_PACKAGE_VERSION "${OPENNS_VERS_MAJOR}.${OPENNS_VERS_MINOR}")
SET (OPENNS_PACKAGE_VERSION_MAJOR "${OPENNS_VERS_MAJOR}.${OPENNS_VERS_MINOR}")
SET (OPENNS_PACKAGE_VERSION_MINOR "${OPENNS_VERS_RELEASE}")
SET (OPENNS_PACKAGE_STRING "${OPENNS_PACKAGE_NAME} ${OPENNS_PACKAGE_VERSION}")
SET (OPENNS_PACKAGE_TARNAME "bag")
SET (OPENNS_PACKAGE_URL "http://www.opennavsurf.org")
SET (VERSION 1.5 OPENNS_PACKAGE_VERSION)

#-----------------------------------------------------------------------------
# Option to Build Shared/Static libs, default is shared
#-----------------------------------------------------------------------------
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
IF (BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
ELSE (BUILD_SHARED_LIBS)
  SET (LIB_TYPE STATIC)
ENDIF (BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# Option to Build against Shared/Static version of HDF5, default is shared
#-----------------------------------------------------------------------------
OPTION (BUILD_WITH_SHARED_HDF5 "Build with HDF5 as a shared library" ON)

# Recurse into the "api" and "examples" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
add_subdirectory (api)
add_subdirectory (examples) 