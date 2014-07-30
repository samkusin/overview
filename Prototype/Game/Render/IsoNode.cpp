//
//  IsoNode.cpp
//  Overview
//
//  Created by Samir Sinha on 7/27/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/Render/IsoNode.hpp"
#include "Game/Render/IsoNodeGraph.hpp"

namespace cinekine { namespace ovengine {
    
    IsoNode::IsoNode(const cinek_bitmap& bitmap,
                     const glm::ivec2& viewPos,
                     const AABB<glm::vec3>& box) :
        _viewPos(viewPos),
        _box(box),
        _behindRange(0,0),
        _renderType(kBitmap),
        _bitmap(bitmap),
        _depth(0),
        _visited(false)
    {
        
    }
    
    void IsoNode::clearBehindList()
    {
        _behindRange.first = _behindRange.second = 0;
    }
    
} /* namespace ovengine */ } /* namespace cinekine */
