//
//  WorldBody.cpp
//  Overview
//
//  Created by Samir Sinha on 11/4/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/WorldObject.hpp"

namespace cinek {
    namespace overview {
    
    WorldObject::WorldObject(const Point& pos,
                             const Point& dir, float speed,
                             void* context) :
        _position(pos),
        _direction(dir),
        _speed(speed),
        _context(context)
    {
    }
    
    void WorldObject::readFromTransform(const btTransform& transform,
                                        const btVector3& halfExtents)
    {
        auto& origin = transform.getOrigin();
        _position.x = origin.x() + halfExtents.x();
        _position.y = origin.y() + halfExtents.y();
        _position.z = origin.z() - halfExtents.z();
        
        auto btDir = transform.getBasis() * kWorldRefDir;
        _direction = toPoint(btDir);
    }
    
    void WorldObject::writeToTransform(btTransform& transform,
                                       const btVector3& halfExtents)
    {
        //  Convert to World Space (target origin is the bbox center.)
        //  Our position is the lower right (max X, max Y, min Z) of
        //  the box.  Yhe "z" extent needs to be adjusted accordingly
        //  to calculate the correct target origin.
        btVector3 worldPos(_position.x - halfExtents.x(),
                           _position.y - halfExtents.y(),
                           _position.z + halfExtents.z());
        transform.setOrigin(worldPos);
        
        // basis back  / MAP Z
        // basis up    / MAP Y
        // basis right / MAP X
        btMatrix3x3 basis;
        basis[2] = btVector3(0,0,1);
        basis[1] = btVector3(_direction.x, _direction.y, _direction.z);
        basis[0] = basis[2].cross(basis[1]);
        transform.setBasis(basis);
    }
    
    void WorldObject::setSpeed(float speed)
    {
        _speed = speed;
    }

    
    } /* namespace overview */
} /* namespace cinek */
