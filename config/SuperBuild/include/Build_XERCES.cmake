#  -*- mode: cmake -*-

#
# Build TPL: XERCES 
#  

# --- Define all the directories and common external project flags
define_external_project_args(XERCES 
                             TARGET xerces
                             BUILD_IN_SOURCE
                             DEPENDS ${MPI_PROJECT} )

set(CFLAGS "${CMAKE_C_FLAGS}")
set(CXXFLAGS "${CMAKE_CXX_FLAGS}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CFLAGS "${CFLAGS} ${CMAKE_C_FLAGS_DEBUG}")
  set(CXXFLAGS "${CXXFLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  set(CFLAGS "${CFLAGS} ${CMAKE_C_FLAGS_MINSIZEREL}")
  set(CXXFLAGS "${CXXFLAGS} ${CMAKE_CXX_FLAGS_MINSIZEREL}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CFLAGS "${CFLAGS} ${CMAKE_C_FLAGS_RELEASE}")
  set(CXXFLAGS "${CXXFLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(CFLAGS "${CFLAGS} ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
  set(CXXFLAGS "${CXXFLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()

# Build the build script
set(XERCES_sh_build ${XERCES_prefix_dir}/xerces-build-step.sh)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/xerces-build-step.sh.in
               ${XERCES_sh_build}
	       @ONLY)

# Configure the CMake command file
set(XERCES_cmake_build ${XERCES_prefix_dir}/xerces-build-step.cmake)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/xerces-build-step.cmake.in
               ${XERCES_cmake_build}
	       @ONLY)

#
#  Hopper cannot build the "samples"
#
if ( DEFINED ENV{NERSC_HOST})
  set(XERCES_CMAKE_COMMAND ${CMAKE_COMMAND} -P ${XERCES_cmake_build})	
else()
  set(XERCES_CMAKE_COMMAND ${MAKE})
endif()

# --- Add external project build and tie to the ZLIB build target
ExternalProject_Add(${XERCES_BUILD_TARGET}
                    DEPENDS   ${XERCES_PACKAGE_DEPENDS}             # Package dependency target
                    TMP_DIR   ${XERCES_tmp_dir}                     # Temporary files directory
                    STAMP_DIR ${XERCES_stamp_dir}                   # Timestamp and log directory
                    # -- Download and URL definitions
                    DOWNLOAD_DIR ${TPL_DOWNLOAD_DIR}                # Download directory
                    URL          ${XERCES_URL}                      # URL may be a web site OR a local file
                    URL_MD5      ${XERCES_MD5_SUM}                  # md5sum of the archive file
                    # -- Configure
                    SOURCE_DIR       ${XERCES_source_dir}           # Source directory
                    CONFIGURE_COMMAND 
		                      <SOURCE_DIR>/configure
				                  --prefix=<INSTALL_DIR> 
						  --with-pic --disable-shared
                                                  CC=${CMAKE_C_COMPILER_USE}
                                                  CFLAGS=${CFLAGS}
                                                  CXX=${CMAKE_CXX_COMPILER_USE}
                                                  CXXFLAGS=${CXXFLAGS}
                    # -- Build
                    BINARY_DIR        ${XERCES_build_dir}           # Build directory 
		    BUILD_COMMAND     ${XERCES_CMAKE_COMMAND}
                    BUILD_IN_SOURCE   ${XERCES_BUILD_IN_SOURCE}     # Flag for in source builds
                    # -- Install
                    INSTALL_DIR      ${TPL_INSTALL_PREFIX}          # Install directory
		    INSTALL_COMMAND  ${MAKE}                        # Install command
                    # -- Output control
                    ${XERCES_logging_args})

include(BuildLibraryName)
build_library_name(xerces-c XERCES_LIBRARY APPEND_PATH ${TPL_INSTALL_PREFIX}/lib)
set(XERCES_LIBRARIES ${XERCES_LIBRARY})
set(XERCES_INCLUDE_DIRS ${TPL_INSTALL_PREFIX}/include)
