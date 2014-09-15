cmake_minimum_required(VERSION 2.8.8)

project( CKGL )

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
     set( PROJECT_COMPILE_FLAGS "-Wall -Wno-missing-braces -std=c++1y -stdlib=libc++" )
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
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Mesh.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderAttributes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLVertexBatch.hpp )

set( PROJECT_GL_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Renderer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Texture.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GL3Mesh.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLShaderLibrary.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/GL/GLVertexBatch.cpp )

source_group( GL FILES ${PROJECT_GL_INCLUDES} ${PROJECT_GL_SOURCES} )

set( PROJECT_SDL_INCLUDES
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLRenderer.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLTexture.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLMesh.hpp )

set( PROJECT_SDL_SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLRenderer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLTexture.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/SDL/SDLMesh.cpp )

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
     ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Graphics2D.cpp )

set( PROJECT_INCLUDES
     ${PROJECT_GL_INCLUDES}
     ${PROJECT_SDL_INCLUDES}
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderDefines.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderTypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderDebug.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/RenderUtils.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Rect.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Style.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapAtlas.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapAtlasLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/BitmapLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Font.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/FontLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Texture.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/RendererCLI.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Graphics2D.hpp )

add_library( CKGL STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )
set_target_properties( CKGL PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS} )

