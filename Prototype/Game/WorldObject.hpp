//
//  WorldObject.hpp
//  Overview
//
//  Created by Samir Sinha on 11/4/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef Overview_Game_WorldObject_hpp
#define Overview_Game_WorldObject_hpp

#include "Game/WorldTypes.hpp"

//  Bullet includes
#include "LinearMath/btTransform.h"

namespace cinek {
    namespace overview {
        
class World;

/// @class WorldObject
/// @brief WorldObject is a wrapper object for the underlying representation of a
///        World body object.
///
class WorldObject
{
public:
    WorldObject() = default;
    WorldObject(const Point& pos, const Point& dir, float speed,
                void* context);
    
    const Point& position() const { return _position; }
    const Point& frontDirection() const { return _direction; }
    float speed() const { return _speed; }
    
    void readFromTransform(const btTransform& transform,
                           const btVector3& halfExtents);
    void writeToTransform(btTransform& transform,
                          const btVector3& halfExtents);
    void setSpeed(float speed);
    
    void* context() { return _context; }
    
private:
    Point _position;
    Point _direction;
    float _speed;
    void* _context;
};
        
    } /* namespace overview */
} /* namespace cinek */

#endif
