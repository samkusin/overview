//
//  StageGenerator.hpp
//  Overview
//
//  Created by Samir Sinha on 8/3/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_StageGenerator_hpp
#define Overview_StageGenerator_hpp

#include "Engine/Builder/BlockLibrary.hpp"
#include "Engine/Model/World.hpp"

#include "cinek/allocator.hpp"

namespace cinekine {
    namespace ovengine {
        class TileLibrary;
        class SpriteLibrary;
    } /* namespace ovengine */
} /* namespace cinekine */


namespace cinekine { namespace prototype {

class StageGenerator
{
public:
    StageGenerator(const Allocator& allocator=Allocator());
    
    struct CreateWorldParams
    {
        int32_t floorX;
        int32_t floorY;
        int32_t overlayToFloorRatio;
        int32_t roomLimit;
    };
    
    std::shared_ptr<ovengine::World> createWorld(const ovengine::TileLibrary& tileLibrary,
                                                 const ovengine::SpriteLibrary& spriteLibrary,
                                                 const CreateWorldParams& params);

private:
    Allocator _allocator;
    
    ovengine::BlockLibrary _blockLibrary;
};


} /* namespace prototype */ } /* namespace cinekine */

#endif
