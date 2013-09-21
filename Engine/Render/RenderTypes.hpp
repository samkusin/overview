//
//  RenderTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 9/21/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_RenderTypes_hpp
#define Overview_RenderTypes_hpp

#include "cinek/rendermodel/types.h"
#include "SDL_rect.h"
#include "SDL_pixels.h"

namespace cinekine {
    namespace ovengine {
    
        typedef uint32_t FontHandle;
        const FontHandle kFontHandle_Invalid = (FontHandle)(-1);
        const FontHandle kFontHandle_Default = (FontHandle)(0);
        
    }   // namespace ovengine
}   // namespace cinekine

#endif
