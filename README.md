#Overview Engine

Basic, incomplete 3D engine.  This code is here to do with as you wish.  It is covered under a mix of ISC and MIT licenses.  Please see the LICENSE for details.

It is meant to showcase personal work.  Over time I may add documentation and support for other platforms.  At the moment I'm porting much of this code over to a new repository that will be better maintained.

If you do decide to build this project, the current OS X only Xcode project is inside the Samples/Common/Apple directory.

Below are some details on build dependencies.

## Dependencies
The current projects are set up to find dependencies inside an "Ext" sibling directory.  Below are some details:

The External packages directory, or "Ext" as described above, should contain the following subdirectories :

* include
* lib
* bin

Our projects point to this folder's root as the location for various external libraries.  For *-nix developers, think of this as an alternative to the /usr/local directory (though if you wish to use /usr/local instead, then you might not need this Ext folder at all.)

This is kept empty so that users can install their versions of the following libraries here:

###Bullet Physics:
http://bulletphysics.org/wordpress/

###BGFX (and BX) Rendering Library
https://github.com/bkaradzic/bgfx
https://github.com/bkaradzic/bx

###SDL2 (for Input and Setup)
https://www.libsdl.org/download-2.0.php

###FreeType
Note : Support for this is minimal at the moment.
https://www.freetype.org

