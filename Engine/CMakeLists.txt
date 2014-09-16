cmake_minimum_required(VERSION 2.8.8)

project( CKOVEngine )

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

set( CINEK_OVENGINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." )

# Component options
#
option( CINEK_OVENGINE_ROCKET_UI "Use the ckRocket (libRocket) UI library" ON )

set( OVENGINE_COMP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Component" )

# Define settings based on supplied options
#
set( OVENGINE_COMP_WINDOW_SOURCE_DIR "${OVENGINE_COMP_SOURCE_DIR}/Window" )

if( CINEK_OVENGINE_ROCKET_UI )
    include_directories( "${CINEK_EXT_PACKAGE_DIR}/Rocket/Include" )
    set( ROCKET_UI_SOURCE_DIR ${OVENGINE_COMP_WINDOW_SOURCE_DIR}/Rocket )
    set( ROCKET_UI_COMPONENT_SOURCES
         ${ROCKET_UI_SOURCE_DIR}/Elements/ElementOverview.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketElementInstancers.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketRenderInterface.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketFileInterface.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketSystemInterface.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketSDLInputMap.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketSDLInput.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketUIWindow.cpp
         ${ROCKET_UI_SOURCE_DIR}/RocketServer.cpp )
    set( ROCKET_UI_COMPONENT_INCLUDES
         ${ROCKET_UI_SOURCE_DIR}/Elements/ElementOverview.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketElementInstancers.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketRenderInterface.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketFileInterface.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketSystemInterface.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketUIWindow.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketSDLInputMap.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketSDLInput.hpp
         ${ROCKET_UI_SOURCE_DIR}/RocketServer.hpp )

    source_group( Component\\Window\\Rocket FILES ${ROCKET_UI_COMPONENT_SOURCES} ${ROCKET_UI_COMPONENT_INCLUDES} )
else( )
    set( ROCKET_UI_COMPONENT_SOURCES )
    set( ROCKET_UI_COMPONENT_INCLUDES )
endif( )

set( COMPONENT_SOURCES
     ${ROCKET_UI_COMPONENT_SOURCES}
     ${OVENGINE_COMP_WINDOW_SOURCE_DIR}/WindowComponent.cpp )
set( COMPONENT_INCLUDES
     ${ROCKET_UI_COMPONENT_INCLUDES}
     ${OVENGINE_COMP_WINDOW_SOURCE_DIR}/WindowComponentCLI.hpp
     ${OVENGINE_COMP_WINDOW_SOURCE_DIR}/WindowComponent.hpp
     ${OVENGINE_COMP_WINDOW_SOURCE_DIR}/Window.hpp )

source_group( Component\\Window FILES ${COMPONENT_SOURCES} ${COMPONENT_INCLUDES} )

# setup build
include_directories( "${CINEK_OVENGINE_ROOT_DIR}"
                     "${CINEK_SDK_ROOT_DIR}"
                     "${CINEK_EXT_PACKAGE_DIR}/include" )

# Build Library
# Define project sources and includes for the parent (if any at this level)
#
# Main
#
set( PROJECT_SOURCES
     ${COMPONENT_SOURCES}
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/AABB.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/ModelCollectionLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/TileCollectionLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Sprite.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteAnimation.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteLibrary.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteInstance.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteLibraryLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/TileGridMap.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Entity.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Room.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Internal/RoomGraphImpl.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/RoomGraph.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/World.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/Block.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/BlockCollection.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/BlockCollectionLoader.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/GridBuilder.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/RoomBuilder.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp )

set( PROJECT_INCLUDES
     ${COMPONENT_INCLUDES}
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/ModelTypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/ModelCollection.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/ModelCollectionLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/ModelLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/AABB.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Tile.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/TileCollection.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/TileCollectionLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/TileLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Sprite.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteAnimation.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteLibraryLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/SpriteInstance.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/ViewModelInstance.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Entity.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/TileGridMap.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/RoomTypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/Room.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/RoomGraph.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/StageTypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Model/World.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/BuilderTypes.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/Block.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/BlockCollection.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/BlockCollectionLoader.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/BlockLibrary.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/GridBuilder.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Builder/RoomBuilder.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Debug.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Defines.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Grid.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/Director.hpp
     ${CMAKE_CURRENT_SOURCE_DIR}/View.hpp
 )

add_library( CKOVEngine STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )
set_target_properties( CKOVEngine PROPERTIES COMPILE_FLAGS ${PROJECT_COMPILE_FLAGS} )

