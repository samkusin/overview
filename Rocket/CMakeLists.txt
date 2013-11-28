#=====================================
# Modified Build script for libRocket
# used by the Cinekine Overview Engine
#=====================================

cmake_minimum_required( VERSION 2.8.8 )

project( RocketUI )

if( WIN32 )
    set( CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES} )
else( )
    set( CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES} )
endif( )

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
     set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -stdlib=libc++" )
else( )
     message( FATAL "Compiler not supported" )
endif( )
if( NOT CINEK_CXX_EXCEPTIONS )
     if( CINEK_CXX_COMPILER_CLANG OR CINEK_CXX_COMPILER_GNU )
          set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
     else( )
          message( FATAL "Compiler not supported" )
     endif( )
endif( )

add_subdirectory(Build)
