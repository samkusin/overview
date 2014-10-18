//
//  GameMapGenerator.hpp
//  Overview
//
//  Created by Samir Sinha on 8/3/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Builder_MapGenerator_hpp
#define Overview_Game_Builder_MapGenerator_hpp

#include "cinek/types.hpp"

namespace cinekine { namespace ovengine {
    class GameTemplates;
} /* namespace ovengine */ } /* namespace cinekine */

namespace cinekine { namespace prototype {
    
    class GameTemplates;
    
    struct GenerateMapParams
    {
        const char* blocksPathname = "";
        int32_t floorX = 32;
        int32_t floorY = 32;
        int32_t overlayToFloorRatio = 1;
        int32_t roomLimit = 1;
    };
    
    ovengine::GameTemplates& generateMapFromTemplates(
                                            ovengine::GameTemplates& gameTemplates,
                                            const GenerateMapParams& params);
    
    
    
} /* namespace prototype */ } /* namespace cinekine */

#endif