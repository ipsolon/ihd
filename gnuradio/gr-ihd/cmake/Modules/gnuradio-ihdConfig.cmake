find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_IHD gnuradio-ihd)

FIND_PATH(
    GR_IHD_INCLUDE_DIRS
    NAMES gnuradio/ihd/api.h
    HINTS $ENV{IHD_DIR}/include
        ${PC_IHD_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_IHD_LIBRARIES
    NAMES gnuradio-ihd
    HINTS $ENV{IHD_DIR}/lib
        ${PC_IHD_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-ihdTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_IHD DEFAULT_MSG GR_IHD_LIBRARIES GR_IHD_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_IHD_LIBRARIES GR_IHD_INCLUDE_DIRS)
