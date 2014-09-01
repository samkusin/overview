//
//  StageGenerator.hpp
//  Overview
//
//  Created by Samir Sinha on 8/3/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_StageGenerator_hpp
#define Overview_StageGenerator_hpp

#include "Engine/Builder/GridBuilder.hpp"
#include "Engine/Builder/BlockLibrary.hpp"

#include "cinek/allocator.hpp"

namespace cinekine { namespace ovengine {
    class Stage;
} /* namespace ovengine */ } /* namespace cinekine */


namespace cinekine { namespace prototype {

class StageGenerator
{
public:
    StageGenerator(ovengine::Stage& stage,
                   const Allocator& allocator=Allocator());
    
    void update();

private:
    Allocator _allocator;
    ovengine::Stage& _stage;
    ovengine::BlockLibrary _blockLibrary;
    ovengine::GridBuilder _floorBuilder;
    ovengine::GridBuilder _overlayBuilder;
};


} /* namespace prototype */ } /* namespace cinekine */

#endif
