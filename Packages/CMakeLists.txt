cmake_minimum_required(VERSION 2.8.8)

project( ovpackages )

# Verify Specified Generator
#
if( ${CMAKE_GENERATOR} STREQUAL "Xcode" )
    if( ${CMAKE_VERSION} VERSION_LESS "2.8.10" )
        message( WARNING "CMake is known to have issues with Xcode project generation prior to version 2.8.10.  Please update your CMake." )
    endif( )
endif( )

add_subdirectory( ext )

set( CINEKSDK_EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext" 
    CACHE FILEPATH "Path to directory containing external packages needed to build projects." )

add_subdirectory( cinek )