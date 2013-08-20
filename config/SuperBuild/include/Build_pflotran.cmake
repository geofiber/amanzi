#  -*- mode: cmake -*-

#
# Build TPL:  PFLOTRAN 
# This builds the chemistry component of PFlotran: pflotranchem.
#   

# --- Define all the directories and common external project flags
define_external_project_args(PFLOTRAN
                             TARGET pflotran
                             BUILD_IN_SOURCE)


# PFlotran needs PETSc.
list(APPEND PFLOTRAN_PACKAGE_DEPENDS ${PETSc_BUILD_TARGET})

# --- Define the CMake configure parameters
# Note:
#      CMAKE_CACHE_ARGS requires -DVAR:<TYPE>=VALUE syntax
#      CMAKE_ARGS -DVAR=VALUE OK
# NO WHITESPACE between -D and VAR. Parser blows up otherwise.
set(PFLOTRAN_CMAKE_CACHE_ARGS
                  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
                  -DCMAKE_INSTALL_PREFIX:STRING=<INSTALL_DIR>
                  -DBUILD_SHARED_LIBS:BOOL=FALSE)

# --- Define the build command

# Build the build script
set(PFLOTRAN_sh_build ${PFLOTRAN_prefix_dir}/pflotran-build-step.sh)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/pflotran-build-step.sh.in
               ${PFLOTRAN_sh_build}
	       @ONLY)

# Configure the CMake command file
set(PFLOTRAN_cmake_build ${PFLOTRAN_prefix_dir}/pflotran-build-step.cmake)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/pflotran-build-step.cmake.in
               ${PFLOTRAN_cmake_build}
	       @ONLY)
set(PFLOTRAN_CMAKE_COMMAND ${CMAKE_COMMAND} -P ${PFLOTRAN_cmake_build})	

# --- Define patch command

# Need Perl to patch
find_package(Perl)
if ( NOT PERL_FOUND )
  message(FATAL_ERROR "Failed to locate perl. "
                      "Can not patch pflotran without PERL")
endif()

# Build the patch script
set(PFLOTRAN_sh_patch ${PFLOTRAN_prefix_dir}/pflotran-patch-step.sh)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/pflotran-patch-step.sh.in
               ${PFLOTRAN_sh_patch}
               @ONLY)

# --- Define the install command

# Build the install script
set(PFLOTRAN_sh_install ${PFLOTRAN_prefix_dir}/pflotran-install-step.sh)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/pflotran-install-step.sh.in
               ${PFLOTRAN_sh_install}
	       @ONLY)

# Configure the CMake command file
set(PFLOTRAN_cmake_install ${PFLOTRAN_prefix_dir}/pflotran-install-step.cmake)
configure_file(${SuperBuild_TEMPLATE_FILES_DIR}/pflotran-install-step.cmake.in
               ${PFLOTRAN_cmake_install}
	       @ONLY)
set(PFLOTRAN_INSTALL_COMMAND ${CMAKE_COMMAND} -P ${PFLOTRAN_cmake_install})	


# --- Add external project build and tie to the PFLOTRAN build target
ExternalProject_Add(${PFLOTRAN_BUILD_TARGET}
                    DEPENDS   ${PFLOTRAN_PACKAGE_DEPENDS}             # Package dependency target
                    TMP_DIR   ${PFLOTRAN_tmp_dir}                     # Temporary files directory
                    STAMP_DIR ${PFLOTRAN_stamp_dir}                   # Timestamp and log directory
                    # -- Download and URL definitions
                    DOWNLOAD_DIR ${TPL_DOWNLOAD_DIR}              # Download directory
                    URL          ${PFLOTRAN_URL}                      # URL may be a web site OR a local file
                    URL_MD5      ${PFLOTRAN_MD5_SUM}                  # md5sum of the archive file
                    # -- Configure
                    SOURCE_DIR       ${PFLOTRAN_source_dir}       # Source directory
		                PATCH_COMMAND sh ${PFLOTRAN_sh_patch}         # Run the patch script
                    CONFIGURE_COMMAND ""
#                    CMAKE_CACHE_ARGS ${PFLOTRAN_CMAKE_CACHE_ARGS}         # CMAKE_CACHE_ARGS or CMAKE_ARGS => CMake configure
#                                     ${Amanzi_CMAKE_C_COMPILER_ARGS}  # Ensure uniform build
#                                     -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
                    # -- Build
#                    BINARY_DIR        ${PFLOTRAN_build_dir}           # Build directory 
                    BUILD_COMMAND     ${PFLOTRAN_CMAKE_COMMAND}            # $(MAKE) enables parallel builds through make
                    BUILD_IN_SOURCE   1     # Flag for in source builds
                    # -- Install
                    INSTALL_DIR      ${TPL_INSTALL_PREFIX}        # Install directory
                    INSTALL_COMMAND   ${PFLOTRAN_INSTALL_COMMAND} 
                    # -- Output control
                    ${PFLOTRAN_logging_args})

include(BuildLibraryName)
build_library_name(pflotranchem PFLOTRAN_LIBRARIES APPEND_PATH ${TPL_INSTALL_PREFIX}/lib)
set(PFLOTRAN_INCLUDE_DIRS ${TPL_INSTALL_PREFIX}/pflotran/src/pflotran)
set(PFLOTRAN_DIR ${TPL_INSTALL_PREFIX}/pflotran)

