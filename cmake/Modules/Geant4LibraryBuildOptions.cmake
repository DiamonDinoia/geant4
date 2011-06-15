# - Setup of general build options for Geant4 Libraries
#
# In addition to the core compiler/linker flags (configured in the MakeRules
# files) for Geant4, its core build can be further configured.
# This configuration is done in this file and includes:
#
#  1) Extra build modes for developers
#  2) Additional compile definitions to assist visualization or optimize
#     performance.
#  3) Whether to build shared and/or static libraries
#  4) Whether to build libraries in global or granular format.
#
#

#------------------------------------------------------------------------------
# Set up Build types or configurations
# If further tuning of compiler flags is needed then it should be done here.
# (It can't be done in the make rules override section).
# However, exercise care when doing this not to override existing flags!!
# We don't do this on WIN32 platforms yet because of some teething issues
# with compiler specifics and linker flags
if(NOT WIN32)
    include(Geant4BuildModes)
endif(NOT WIN32)


#----------------------------------------------------------------------------
# Optional define flags which are applicable globally
#
# - G4_STORE_TRAJECTORY
# ON by default, switching off can improve performance. Needs to be on
# for visualization to work fully. Mark as advanced because most users
# should not need to worry about it.
# FIXES : Bug #1208
option(GEANT4_BUILD_STORE_TRAJECTORY "Store trajectories in event processing.
Switch off for improved performance but note that visualization of trajectories will not be possible" ON)
mark_as_advanced(GEANT4_BUILD_STORE_TRAJECTORY)

if(GEANT4_BUILD_STORE_TRAJECTORY)
    add_definitions(-DG4_STORE_TRAJECTORY)
endif()


# - G4VERBOSE
# ON by default, switching off can improve performance, but at the cost of
# fewer informational or warning messages. Mark as advanced because most users
# should not need to worry about it.
option(GEANT4_BUILD_VERBOSE_CODE 
    "Enable verbose output from Geant4 code. Switch off for better performance at the cost of fewer informational messages or warnings"
    ON)
mark_as_advanced(GEANT4_BUILD_VERBOSE_CODE)

if(GEANT4_BUILD_VERBOSE_CODE)
    add_definitions(-DG4VERBOSE)
endif()



#----------------------------------------------------------------------------
# Setup Library Format Option.
# Libraries can be built in one of two 'formats':
#  global   : One library per category.
#  granular : One library per module.
#
# This division does not always apply because some libraries are the same
# in both 'formats', e.g. G4materials or G4OpenGL.
# Global libraries are built by default, but we provide an option to switch to
# granular format. Granular format is only intended for developers, so we
# mark this option as advanced.
option(GEANT4_BUILD_GRANULAR_LIBS "Build Geant4 with granular libraries" OFF)
mark_as_advanced(GEANT4_BUILD_GRANULAR_LIBS)
GEANT4_ADD_FEATURE(GEANT4_BUILD_GRANULAR_LIBS "Build granular Geant4 libraries")


#----------------------------------------------------------------------------
# Setup Shared and/or Static Library builds
# We name these options without a 'GEANT4_' prefix because they are really
# higher level CMake options.
# Default to static libraries on win32 until declspec issues resolved
# TODO: Fix DLLs on WIN32, then always build shared libs with static as an
# option.
if(WIN32)
    option(BUILD_STATIC_LIBS "Build Geant4 static libraries" ON)
    option(BUILD_SHARED_LIBS "Build Geant4 shared libraries" OFF)
else(WIN32)
    option(BUILD_STATIC_LIBS "Build Geant4 static libraries" OFF)
    option(BUILD_SHARED_LIBS "Build Geant4 shared libraries" ON)
endif(WIN32)

# Because both could be switched off accidently, FATAL_ERROR if neither
# option has been selected.
if(NOT BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS)
    message(FATAL_ERROR "Neither static nor shared libraries will be built")
endif()

# On WIN32, we need to build the genwindef application to create export
# def file for building DLLs.
# We only use it as a helper application at the moment so we exclude it from 
# the ALL target.
# TODO: We could move this section into the Geant4MacroLibraryTargets.cmake
# if it can be protected so that the genwindef target wouldn't be defined
# more than once... Put it here for now...
if(WIN32)
    # Assume the sources are co-located 
    get_filename_component(_genwindef_src_dir ${CMAKE_CURRENT_LIST_FILE} PATH)
    add_executable(genwindef EXCLUDE_FROM_ALL 
        ${_genwindef_src_dir}/genwindef/genwindef.cpp 
        ${_genwindef_src_dir}/genwindef/LibSymbolInfo.h
        ${_genwindef_src_dir}/genwindef/LibSymbolInfo.cpp)
endif()


#----------------------------------------------------------------------------
# Setup Locations for Build Outputs
# Because of the highly nested structure of Geant4, targets will be distributed
# throughout this tree, potentially making usage and debugging difficult
# (especially if developers use non-CMake tools).
# We therefore set the output directory of runtime, library and archive
# targets to some low level directories under the build tree.
# Note that for multi-configuration generators like VS and Xcode, these
# directories will have the configuration type (e.g. Debug) appended to them.
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/runtime)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/library)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/outputs/archive)


