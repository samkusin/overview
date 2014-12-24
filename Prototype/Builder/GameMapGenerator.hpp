//
//  GameMapGenerator.hpp
//  Overview
//
//  Created by Samir Sinha on 8/3/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_Builder_MapGenerator_hpp
#define Overview_Game_Builder_MapGenerator_hpp

#include "Shared/StaticWorldMap.hpp"

namespace cinek { namespace overview {

    struct GenerateMapParams
    {
        const char* blocksPathname = "";
        int32_t floorX = 32;
        int32_t floorY = 32;
        int32_t overlayToFloorRatio = 1;
        int32_t roomLimit = 1;
    };

    unique_ptr<StaticWorldMap> generateMapFromTemplates(
            const GameTemplates& gameTemplates,
            const GenerateMapParams& params);

} /* namespace overview */ } /* namespace cinek */

#endif