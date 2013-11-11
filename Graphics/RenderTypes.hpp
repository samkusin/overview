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
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_pixels.h"

namespace cinekine {
    namespace glx {

    typedef uint32_t FontHandle;
    /** An invalid font handle */
    const FontHandle kFontHandle_Invalid = (FontHandle)(-1);
    /** The 'default' found */
    const FontHandle kFontHandle_Default = (FontHandle)(0);

    }   // namespace glx
}   // namespace cinekine

#endif
