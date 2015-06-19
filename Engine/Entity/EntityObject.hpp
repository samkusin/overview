//
//  EntityObject.hpp
//  Overview
//
//  Created by Samir Sinha on 5/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Object_hpp
#define Overview_Entity_Object_hpp

#include "EntityTypes.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace overview {

class EntityObject
{
    CK_CLASS_NON_COPYABLE(EntityObject);
    
public:
    EntityObject() = default;
    EntityObject(EntityId eid, const Allocator& allocator=Allocator());
    EntityObject(EntityObject&& other);
    EntityObject& operator=(EntityObject&& other);
    
    operator bool() const {
        return _eid != kNullEntity;
    }
    EntityId id() const {
        return _eid;
    }
    
    const Matrix4& matrix() const {
        return _matrix;
    }
    Matrix4& matrix() {
        return _matrix;
    }
    
    ComponentRowIndex componentIndexFromId(ComponentId id) const;

private:
    friend class EntityDatabase;
    
    void addComponent(ComponentId id, ComponentRowIndex index);
    ComponentRowIndex removeComponent(ComponentId id);

    EntityId _eid = kNullEntity;
    Matrix4 _matrix;
    
    vector<std::pair<ComponentId, ComponentRowIndex>> _components;
};


} /* namespace overview */ } /* namespace cinek */

#endif
