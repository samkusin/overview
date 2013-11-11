cmake_minimum_required(VERSION 2.8.8)

project( OVEngine )

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
set( PROJECT_COMPILE_FLAGS "-Wall -std=c++11 -stdlib=libc++" )
if( NOT CINEK_CXX_EXCEPTIONS )
    set( PROJECT_COMPILE_FLAGS "${PROJECT_COMPILE_FLAGS} -fno-exceptions -fno-rtti")
endif( )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CINEK_SDK_ROOT_DIR}" "${CINEK_EXT_PACKAGE_DIR}/include" )

# Build Library
# Define project sources and includes for the parent (if any at this level)
#
# Main
#
set( PROJECT_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIView.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SpriteDatabaseLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/TileDatabaseLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Theater.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Templates.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp )

set( PROJECT_INCLUDES 
     ${CMAKE_CURRENT_SOURCE_DIR}/Debug.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Defines.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Director.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SpriteDatabaseLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Theater.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/TheaterClient.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/TileDatabaseLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIView.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/View.hpp )

add_library( ckOVEngine STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )
set_target_properties( ckOVEngine PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS} )

