cmake_minimum_required(VERSION 2.8.8)

project( CKUI )

###############################################################################
# CMake custom variables used to set up the build process.  All subtargets
# will use these variables to discover packages, set compile settings, or other
# modify other build related settings.

# Setup the external package directory used for compiling the SDK.
if ( NOT DEFINED CINEK_EXT_PACKAGE_DIR )
    set( CINEK_EXT_PACKAGE_DIR ""
         CACHE FILEPATH "The path to external packages used by the SDK for compilation." )
endif( )

option( CINEK_CXX_EXCEPTIONS 
    "Enable C++ exception handling" OFF 
)

# Setup compile options
set( PROJECT_COMPILE_FLAGS "-std=c++11 -Wall -Wno-missing-braces -std=c++11 -stdlib=libc++" )
if( NOT CINEK_CXX_EXCEPTIONS )
    set( PROJECT_COMPILE_FLAGS "${PROJECT_COMPILE_FLAGS} -fno-exceptions -fno-rtti" )
endif( )

# Build Library
# Define project sources and includes for the parent (if any at this level)
#
set( PROJECT_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIFrame.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIGfx.cpp )

set( PROJECT_INCLUDES
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUITypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIGfx.hpp 
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIFrame.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIClient.hpp )

add_library( ckUI STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )
set_source_files_properties( ${PROJECT_SOURCES} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS} )
#set_target_properties( CKUI PROPERTIES LINK_FLAGS "-stdlib=libc++" )
