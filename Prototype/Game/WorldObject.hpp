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
    WorldObject(const WorldObjectTransform& transform, void* context);
    
    void setId(SimObjectId objId) { _objectId = objId; }
    SimObjectId id() const { return _objectId; }
    
    const WorldObjectTransform& transform() const { return _transform; }
    void setTransform(const WorldObjectTransform& transform) { _transform = transform; }
    void applyTransform(const WorldObjectTransform& transform);

    void* context() { return _context; }
    
    bool readFromTransform(const btTransform& transform,
                           const btVector3& halfExtents);
    void writeToTransform(btTransform& transform,
                          const btVector3& halfExtents) const;
    
private:
    WorldObjectTransform _transform;
    SimObjectId _objectId;
    void* _context;
};
        
    } /* namespace overview */
} /* namespace cinek */

#endif
