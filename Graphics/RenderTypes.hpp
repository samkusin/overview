//
//  RenderTypes.hpp
//  Graphics
//
//  Created by Samir Sinha on 9/21/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_RenderTypes_hpp
#define CK_Graphics_RenderTypes_hpp

#include "cinek/types.hpp"

//  commonly used glm headers
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace cinekine {
    namespace glx {

    /**< Defines a RGBA quad */
    typedef glm::u8vec4 RGBAColor;

    /** Identifies a loaded font */
    typedef uint32_t FontHandle;
    /** An invalid font handle */
    const FontHandle kFontHandle_Invalid = (FontHandle)(-1);
    /** The 'default' found */
    const FontHandle kFontHandle_Default = (FontHandle)(0);

    }   // namespace glx
}   // namespace cinekine

#endif
