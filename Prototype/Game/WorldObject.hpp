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
class WorldObject : public WorldObjectBase
{
public:
    WorldObject();
    WorldObject(const WorldObjectTransform& transform, void* context);
    
    void setId(GameObjectId objId) { _objectId = objId; }
    GameObjectId id() const { return _objectId; }
    
    const WorldObjectTransform& transform() const { return _transform; }
    void setTransform(const WorldObjectTransform& transform) { _transform = transform; }
    void applyTransform(const WorldObjectTransform& transform);
    
    bool readFromTransform(const btTransform& transform,
                           const btVector3& halfExtents);
    void writeToTransform(btTransform& transform,
                          const btVector3& halfExtents) const;
    
private:
    WorldObjectTransform _transform;
    GameObjectId _objectId;
};
        
    } /* namespace overview */
} /* namespace cinek */

#endif
