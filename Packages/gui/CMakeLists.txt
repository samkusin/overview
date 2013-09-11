cmake_minimum_required(VERSION 2.8.8)

project( imgui )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -stdlib=libc++" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti" )

set( PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiRenderGL2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiRenderGL3.h
)

set( PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiRenderGL2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiRenderGL3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
)

# Targets
add_library( imgui STATIC ${PROJECT_INCLUDES} ${PROJECT_SOURCES} )

# Projects
# add_subdirectory( test )

