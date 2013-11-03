cmake_minimum_required(VERSION 2.8.10)

project( overviewsdk )

# 
#   Verification (Packages, etc.)
#
if( ${CMAKE_GENERATOR} STREQUAL "Xcode" )
    if( ${CMAKE_VERSION} VERSION_LESS "2.8.10" )
        message( WARNING "CMake is known to have issues with Xcode project generation prior to version 2.8.10.  Please update your CMake to version 2.8.10 or later." )
    endif( )
endif( )

#   Global build setup
#
set( CINEK_EXT_PACKAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Packages"
     CACHE FILEPATH "Path to directory containing external packages needed to build projects" )

set( CINEK_CXX_EXCEPTIONS OFF CACHE BOOL "Disable C++ exception handling where applicable" )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

#   All subprojects ordered by dependency (i.e. Engine needs UI and cinek, and Prototype needs them all)

#   Libraries to build
#
add_subdirectory( cinek )
add_subdirectory( UI )
add_subdirectory( Engine )

#   Build the remaining source in one sweep as an executable - all source from subdirectories is included in the
#   final project.
#
set( PROJECT_INCLUDES )
set( PROJECT_SOURCES )

set( PROJECT_MODULES "Prototype" )

foreach( PROJECT_MODULE ${PROJECT_MODULES} )
    if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MODULE}" )
        message( STATUS "\tProject directory found '${PROJECT_MODULE}'" )

        set( SOURCE_PATHS 
             ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MODULE}/*.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MODULE}/*.c )
        set( INCLUDE_PATHS
             ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MODULE}/*.hpp
             ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MODULE}/*.h )

        file( GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES_PATHS} )
        file( GLOB_RECURSE INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_PATHS} )
        
        set( PROJECT_INCLUDES ${PROJECT_INCLUDES} ${INCLUDES} )
        set( PROJECT_SOURCES ${PROJECT_SOURCES} ${SOURCES} )
    else( )
        message( STATUS "\tProject directory not found '${PROJECT_MODULE}' (ignoring)" )
    endif( )
endforeach( PROJECT_MODULE )

# Build Executable
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# Apple OSX
# add_executable( ovengine MACOSX_BUNDLE ${PROJECT_SOURCES} )
