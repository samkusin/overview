//
//  SimulationTypes.hpp
//  Overview
//
//  Created by Samir Sinha on 11/22/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_SimulationTypes_hpp
#define Overview_Game_SimulationTypes_hpp

#include "Engine/Model/ModelTypes.hpp"

namespace cinek { namespace overview {

using SimObjectId = uint32_t;
using EntityId = SimObjectId;

struct SimDebugMessage
{
    enum
    {
        kDrawLine,
        kDrawPoint
    }
    type;
    
    Point p0;
    Point p1;
    glm::vec3 color;
};
    
struct WorldObjectTransform
{
    Point pos;
    Point dir;
    float speed;
    
    enum Attribute
    {
        kPosition    = (1 << 0),
        kDirection   = (1 << 1),
        kSpeed       = (1 << 2)
    };
    
    WorldObjectTransform() : speed(0.f), _dirtyFlags(0) {}
    WorldObjectTransform(const Point& pos_, const Point& dir_, float speed_);

    void setDirty(Attribute attr);
    bool isDirty(Attribute attr) const;
    void clearDirtyFlags() { _dirtyFlags = 0; }
    
    WorldObjectTransform& operator|=(const WorldObjectTransform& transform);
    
private:
    uint32_t _dirtyFlags;
};

inline WorldObjectTransform::WorldObjectTransform(
    const Point& pos_,
    const Point& dir_,
    float speed_
) :
    pos(pos_),
    dir(dir_),
    speed(speed_),
    _dirtyFlags(kPosition | kDirection | kSpeed)
{
}

inline bool WorldObjectTransform::isDirty(Attribute attr) const
{
    return (_dirtyFlags & static_cast<uint32_t>(attr)) != 0;
}

inline void WorldObjectTransform::setDirty(Attribute attr)
{
    _dirtyFlags |= static_cast<uint32_t>(attr);
}

} /* namespace overview */ } /* namespace cinek */

#endif
