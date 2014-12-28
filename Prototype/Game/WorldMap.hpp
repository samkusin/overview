//
//  WorldMap.hpp
//  Overview
//
//  Created by Samir Sinha on 12/24/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Model_WorldMap_hpp
#define Overview_Model_WorldMap_hpp

#include "Game/WorldTypes.hpp"
#include "Engine/Model/TileGridMap.hpp"

namespace cinek { namespace overview {

class StaticWorldMap;

//  An object representing the world static map grid within a World
//  object.

class WorldMap : public WorldObjectBase
{
    CK_CLASS_NON_COPYABLE(WorldMap);
    
public:
    WorldMap(const StaticWorldMap& worldMap);
    
    AABB<Point> bounds() const;
    
private:
    const StaticWorldMap& _worldMap;
};

} /* namespace overview */ } /* namespace cinek */

#endif
