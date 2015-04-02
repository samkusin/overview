/**
 * \file    Model/StageTypes.hpp
 *
 * Shared map data object containing terrain and prop objects.
 *
 * \note    Created by Samir Sinha on 3/2/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Model_StageTypes_hpp
#define Overview_Model_StageTypes_hpp

#include "Engine/Model/ModelTypes.hpp"

namespace cinek {
    namespace overview {
        class SpriteInstance;
    }
}

namespace cinek {
    namespace overview {

using SpriteInstancePtr = SpriteInstance*;
using ConstSpriteInstancePtr = const SpriteInstance*;

    } /* overview */
} /* cinekine */

#endif
