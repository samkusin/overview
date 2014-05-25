cmake_minimum_required(VERSION 2.8.8)

project( CKCore )

###############################################################################
# CMake custom variables used to set up the build process.  All subtargets
# will use these variables to discover packages, set compile settings, or other
# modify other build related settings.

# Setup the external package directory used for compiling the SDK.
if ( NOT DEFINED CINEK_EXT_PACKAGE_DIR )
    set( CINEK_EXT_PACKAGE_DIR ""
         CACHE FILEPATH "The path to external packages used by the SDK for compilation." )
endif( )

if ( NOT DEFINED CINEK_SDK_ROOT_DIR )
     set( CINEK_SDK_ROOT_DIR ""
          CACHE FILEPATH "The path to the CineK SDK root folder." )
endif( )

option( CINEK_CXX_EXCEPTIONS
    "Enable C++ exception handling" OFF
)

# Setup compile options
#
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  set( CINEK_CXX_COMPILER_CLANG TRUE )
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  set( CINEK_CXX_COMPILER_GNU TRUE )
elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
  set( CINEK_CXX_COMPILER_MSVC TRUE )
endif( )

if( CINEK_CXX_COMPILER_CLANG )
     set( PROJECT_COMPILE_FLAGS "-Wall -Wno-missing-braces -std=c++11 -stdlib=libc++" )
else( )
     message( FATAL "Compiler not supported" )
endif( )
if( NOT CINEK_CXX_EXCEPTIONS )
     if( CINEK_CXX_COMPILER_CLANG OR CINEK_CXX_COMPILER_GNU )
          set( PROJECT_COMPILE_FLAGS "${PROJECT_COMPILE_FLAGS} -fno-exceptions -fno-rtti")
     else( )
          message( FATAL "Compiler not supported" )
     endif( )
endif( )

include_directories( "${CINEK_SDK_ROOT_DIR}" "${CINEK_EXT_PACKAGE_DIR}/include" )

# Build Library
# Define project sources and includes for the parent (if any at this level)
#

# Stream
#
set( PROJECT_STREAM_INCLUDES
     ${CMAKE_CURRENT_SOURCE_DIR}/FileStreamBuf.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/StreamBufRapidJson.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/JsonUtilities.hpp )
set( PROJECT_STREAM_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/FileStreamBuf.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/JsonUtilities.cpp )

# Main
#
set( PROJECT_SOURCES
     ${PROJECT_STREAM_SOURCES} )

set( PROJECT_INCLUDES
     ${PROJECT_STREAM_INCLUDES} )

add_library( CKCore STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )
set_target_properties( CKCore PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS} )

