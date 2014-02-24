/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * @file    TheaterCLI.hpp
 * @author  Samir Sinha
 * @date    8/22/2013
 * @brief   The Theater Client Interface exposed to applications
 * @copyright Cinekine
 */

#ifndef Overview_TheaterCLI_hpp
#define Overview_TheaterCLI_hpp

#include <cinek/framework/types.hpp>

namespace cinekine {
    namespace glx {
        class BitmapLibrary;
    }
    namespace rendermodel {
        class TileDatabase;
        class SpriteDatabase;
    }
}

namespace cinekine {
    namespace ovengine {
    
    //  The Client Interface (i.e to the Director object)
    class TheaterCLI
    {
    public:
        virtual ~TheaterCLI() {}
        
        /**
         *  Loads data into the current SpriteDatabase.
         *  @param spriteDbName  The name of the DB (this translates to a JSON
         *                       source file of the same name.
         *  @param bitmapLibrary The bitmap library to load sprites into
         */
        virtual void loadSpriteDatabase(const char* spriteDbName,
                                        glx::BitmapLibrary& bitmapLibrary) = 0;
        /**
         *  Loads a tile database (including bitmaps) into the global tile 
         *  database.
         *  @param tileDbName    The name of the DB (this translates to a JSON
         *                       source file of the same name.
         *  @param bitmapLibrary The bitmap library to load tiles into
         */
        virtual void loadTileDatabase(const char* tileDbName,
                                      glx::BitmapLibrary& bitmapLibrary) = 0;
        /**
         *  @return The read-only Tile database
         */
        virtual const rendermodel::TileDatabase& tileDatabase() const = 0;
        /**
         *  @return The read-only Sprite database
         */
        virtual const rendermodel::SpriteDatabase& spriteDatabase() const = 0;
    };
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
