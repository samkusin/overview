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

option( CINEK_CXX_EXCEPTIONS 
    "Enable C++ exception handling" OFF 
)

# Setup compile options
set( PROJECT_COMPILE_FLAGS "-Wall -std=c++11 -stdlib=libc++" )
if( NOT CINEK_CXX_EXCEPTIONS )
    set( PROJECT_COMPILE_FLAGS "${PROJECT_COMPILE_FLAGS} -fno-exceptions -fno-rtti")
endif( )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

# Build Library
# Define project sources and includes for the parent (if any at this level)
#
set( PROJECT_C_SOURCES 
     ${CMAKE_CURRENT_SOURCE_DIR}/Utils/stb/stb.c )

set( PROJECT_PLATFORM_INCLUDES
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/SDL/SDLTexture.hpp )

set( PROJECT_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/SDL/SDLTexture.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/BitmapAtlas.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/BitmapAtlasLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/BitmapLibrary.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/Font.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/FontLibrary.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/Stylus.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/Renderer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Stream/FileStreamBuf.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIView.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SpriteDatabaseLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/TileDatabaseLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Theater.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Templates.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp )

set( PROJECT_INCLUDES 
     ${PROJECT_PLATFORM_INCLUDES}
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/RenderTypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/BitmapAtlas.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/BitmapAtlasLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/BitmapLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/Font.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/FontLibrary.hpp 
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/Stylus.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/Texture.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Render/Renderer.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Stream/FileStreamBuf.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Utils/stb/stb_truetype.h
     ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Math.hpp 
     ${CMAKE_CURRENT_SOURCE_DIR}/Utils/StreamBufRapidJson.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Debug.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Defines.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Director.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SpriteDatabaseLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Theater.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/TheaterClient.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/TileDatabaseLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/IMGUIView.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/View.hpp )

add_library( ckOVEngine STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} ${PROJECT_C_SOURCES} )
set_source_files_properties( ${PROJECT_SOURCES} PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS} )
set_source_files_properties( ${PROJECT_C_SOURCES} PROPERTIES COMPILE_FLAGS "-Wall -std=c99" )
