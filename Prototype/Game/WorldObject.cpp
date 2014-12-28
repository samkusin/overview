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
    
    WorldObject::WorldObject() :
        WorldObjectBase(ClassId::kWorldObject)
    {
    }
    
    WorldObject::WorldObject(const WorldObjectTransform& transform,
                             void* context) :
        WorldObjectBase(ClassId::kWorldObject),
        _transform(transform),
        _context(context)
    {
    }
    
    void WorldObject::applyTransform(const WorldObjectTransform& transform)
    {
        _transform |= transform;
    }
    
    bool WorldObject::readFromTransform(const btTransform& transform,
                                        const btVector3& halfExtents)
    {
        bool same = true;
        auto& origin = transform.getOrigin();
        Point newPos(origin.x() + halfExtents.x(),
                     origin.y() + halfExtents.y(),
                     origin.z() - halfExtents.z());
        if (newPos != _transform.pos)
        {
            _transform.pos = newPos;
            same = false;
        }
        
        auto newDir = toPoint(transform.getBasis() * kWorldRefDir);
        if (newDir != _transform.dir)
        {
            _transform.dir = newDir;
            same = false;
        }
        
        if (_transform.isDirty(WorldObjectTransform::kSpeed))
        {
            same = false;
        }
        
        //  reset dirty flags since we've applied the actual position
        //  and direction to this WorldObject from our world.
        _transform.clearDirtyFlags();
        return same;
    }
    
    void WorldObject::writeToTransform(btTransform& transform,
                                       const btVector3& halfExtents) const
    {
        //  Convert to World Space (target origin is the bbox center.)
        //  Our position is the lower right (max X, max Y, min Z) of
        //  the box.  Yhe "z" extent needs to be adjusted accordingly
        //  to calculate the correct target origin.
        
        if (_transform.isDirty(WorldObjectTransform::kPosition))
        {
            transform.setOrigin(translateToBtPosition(_transform.pos, halfExtents));
        }
        
        if (_transform.isDirty(WorldObjectTransform::kDirection))
        {
            // basis back  / MAP Z
            // basis up    / MAP Y
            // basis right / MAP X
            btMatrix3x3 basis = orientToBtDirection(_transform.dir);
            transform.setBasis(basis);
        }
    }

    
    } /* namespace overview */
} /* namespace cinek */
