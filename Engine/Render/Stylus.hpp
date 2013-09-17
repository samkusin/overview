//
//  Render/Stylus.hpp
//  Overview
//
//  Created by Samir Sinha on 9/16/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_Renderer_Stylus_hpp
#define Overview_Renderer_Stylus_hpp

#include "Font.hpp"

#include "cinek/rendermodel/types.h"
#include "SDL_rect.h"

namespace cinekine {
    namespace ovengine {

    class BitmapAtlas;    
    class Renderer;

    //  Handles drawing elements to the Rendering Device
    class Stylus
    {
    public:    
        Stylus(Renderer& renderer);
        ~Stylus();

        //  returns the current viewport rect
        SDL_Rect getViewport() const;
        //  set drawing resources
        void setBitmapAtlas(cinek_bitmap_atlas atlas);
        void setFont(FontHandle fontHandle);
        //  Draws a bitmap onto the current screen.
        void drawBitmap(cinek_bitmap_index bitmapIndex, int32_t x, int32_t y);

    private:
        Renderer& _renderer;
        cinek_bitmap_atlas _atlasIndex;
        const BitmapAtlas* _atlas;
        FontHandle _fontHandle;
    };

    }   // namespace ovengine
}   // namespace cinekine


#endif
