//
//  Render/Texture.hpp
//  Overview
//
//  Created by Samir Sinha on 9/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_Texture_hpp
#define CK_Graphics_Texture_hpp

#include "cinek/rendermodel/types.h"

namespace cinekine {
    namespace glx {
        
    class Texture
    {
    public:
        virtual ~Texture() {}
        // used to evaluate Texture validity.  Best used when evaluating
        // whether a texture was succesfully created. 
        virtual operator bool() const = 0;
    };
    
    
    }   // namespace glx
}   // namespace cinekine

#endif
