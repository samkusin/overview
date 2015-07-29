//
//  Transform.hpp
//  Overview
//
//  Created by Samir Sinha on 5/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Entity_Component_Transform_hpp
#define Overview_Entity_Component_Transform_hpp

#include "Engine/Entity/EntityTypes.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace cinek { namespace overview {

namespace component
{
    struct Transform
    {
        COMPONENT_DEFINITION(Transform, void);
        
        void setLocalPosition(const glm::dvec3& pos) {
            _localPosition = pos;
            _isDirty = true;
        }
        const glm::dvec3& localPosition() const {
            return _localPosition;
        }
        bool dirty() const {
            return _isDirty;
        }

        const glm::dmat4& worldSRT() const {
            return _worldSRT;
        }
        
        void calculateWorldSRTFromParent(const glm::dmat4& parent);

        void setDirty(bool dirty) {
            _isDirty = dirty;
        }
        
        void setParent(Entity e) {
            _parent = e;
            _isDirty = true;
        }
        void setChild(Entity e) { _firstChild = e; }
        void setSibling(Entity e) { _nextSibling = e; }
        
        Entity parent() const { return _parent; }
        Entity child() const { return _firstChild; }
        Entity sibling() const { return _nextSibling; }
        
    private:
        //  local components for the entity owning this Transform
        ckm::vec3 _localPosition;
        //  the SRT matrix used as the world space representation
        ckm::mat4 _worldSRT;
        
        Entity _parent;
        Entity _firstChild;
        Entity _nextSibling;
        
        //  flags used during Transform calculations
        bool _isDirty;
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
