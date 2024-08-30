find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_CHAMELEON gnuradio-chameleon)

FIND_PATH(
    GR_CHAMELEON_INCLUDE_DIRS
    NAMES gnuradio/chameleon/api.h
    HINTS $ENV{CHAMELEON_DIR}/include
        ${PC_CHAMELEON_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_CHAMELEON_LIBRARIES
    NAMES gnuradio-chameleon
    HINTS $ENV{CHAMELEON_DIR}/lib
        ${PC_CHAMELEON_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-chameleonTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_CHAMELEON DEFAULT_MSG GR_CHAMELEON_LIBRARIES GR_CHAMELEON_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_CHAMELEON_LIBRARIES GR_CHAMELEON_INCLUDE_DIRS)
