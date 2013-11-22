cmake_minimum_required(VERSION 2.8.8)

project( CKGraphics )

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
set( PROJECT_COMPILE_FLAGS "-Wall -Wno-missing-braces -std=c++11 -stdlib=libc++" )
if( NOT CINEK_CXX_EXCEPTIONS )
    set( PROJECT_COMPILE_FLAGS "${PROJECT_COMPILE_FLAGS} -fno-exceptions -fno-rtti")
endif( )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/.." "${CINEK_SDK_ROOT_DIR}" "${CINEK_EXT_PACKAGE_DIR}/include" )

# Build Library
# Define project sources and includes for the parent (if any at this level)
#

# Renderer
#

# External Packages for the Renderer
#
set( PROJECT_EXTERNAL_STB_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/External/stb/stb.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/External/stb/stb_truetype.h )

source_group( External\\stb FILES ${PROJECT_EXTERNAL_STB_SOURCES} )

set( PROJECT_RENDER_EXTERNAL_SOURCES )

source_group( External FILES ${PROJECT_EXTERNAL_SOURCES} )

# Backend Implementations
#
set( PROJECT_GL_INCLUDES
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLUtils.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Renderer.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Texture.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderAttributes.hpp )

set( PROJECT_GL_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Renderer.cpp 
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Texture.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLibrary.cpp )

source_group( GL FILES ${PROJECT_GL_INCLUDES} ${PROJECT_GL_SOURCES} )

set( PROJECT_SDL_INCLUDES
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLRenderer.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLTexture.hpp )

set( PROJECT_SDL_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLRenderer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLTexture.cpp )

source_group( SDL FILES ${PROJECT_SDL_INCLUDES} ${PROJECT_SDL_SOURCES} )

# Main
#
set( PROJECT_SOURCES
     ${PROJECT_EXTERNAL_SOURCES}
     ${PROJECT_EXTERNAL_STB_SOURCES}
     ${PROJECT_GL_SOURCES}
     ${PROJECT_SDL_SOURCES} 
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapAtlas.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapAtlasLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapLibrary.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Font.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/FontLibrary.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp )

set( PROJECT_INCLUDES
     ${PROJECT_GL_INCLUDES}
     ${PROJECT_SDL_INCLUDES}
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderDefines.hpp 
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderTypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderDebug.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderUtils.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/JSON.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Rect.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Style.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapAtlas.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapAtlasLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Font.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/FontLibrary.hpp 
     ${CMAKE_CURRENT_SOURCE_DIR}/Texture.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.hpp )

add_library( ckGL STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )
set_target_properties( ckGL PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS} )

