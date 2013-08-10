cmake_minimum_required(VERSION 2.8.8)

project( ext )

# Projects
# Enumerate which projects to build based on the existence and contents of modules.txt
#
message( STATUS "Enumering Extenral modules for building:" )

if( NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/modules.txt" )
    message( WARNING "The modules.txt file was not found.  Building no additional modules." )
else()
    file( STRINGS modules.txt EXT_MODULES )

    foreach( EXT_MODULE ${EXT_MODULES} )
        if ( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${EXT_MODULE}" )
            message( STATUS "\tModule directory found '${EXT_MODULE}'" )
            add_subdirectory( ${EXT_MODULE} )
        else( )
            message( STATUS "\tModule directory not found '${EXT_MODULE}' (ignoring)" )
        endif( )
    endforeach( EXT_MODULE )
endif( )
