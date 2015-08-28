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
#include "Engine/EngineMath.hpp"

namespace cinek { namespace overview {

namespace component
{
    struct Transform
    {
        COMPONENT_DEFINITION(Transform);
        
        void setLocalPosition(const ckm::vec3& pos) {
            _localPosition = pos;
            _isDirty = true;
        }
        const ckm::vec3& localPosition() const {
            return _localPosition;
        }
        void setLocalOrient(const ckm::quat& q) {
            _localOrient = q;
            _isDirty = true;
        }
        const ckm::quat& localOrient() const {
            return _localOrient;
        }
        void setParent(Entity e) {
            _parent = e;
            _isDirty = true;
        }
        const ckm::vec3& position() const {
            return _position;
        }
        const ckm::quat& orient() const {
            return _orient;
        }
        
        void setChild(Entity e) { _firstChild = e; }
        void setSibling(Entity e) { _nextSibling = e; }
        
        Entity parent() const { return _parent; }
        Entity child() const { return _firstChild; }
        Entity sibling() const { return _nextSibling; }

        ckm::mat4& calcLocalMatrix(ckm::mat4& srt) const;
        ckm::mat4& calcMatrix(ckm::mat4& srt) const;
        ckm::mat3& calcLocalBasis(ckm::mat3& basis) const;
        ckm::mat3& calcBasis(ckm::mat3& basis) const;
        
    private:
        void setPosition(const ckm::vec3& pos) {
            _position = pos;
        }
        void setOrient(const ckm::quat& q) {
            _orient = q;
        }
        void clearDirty() { _isDirty = false; }
        void setDirty() { _isDirty = true; }
        bool isDirty() const { return _isDirty; }
        
    private:
        friend struct UpdateTransform;
        
        //  local components for the entity owning this Transform
        ckm::vec3 _localPosition;
        ckm::quat _localOrient;
        
        //  calculated global values
        ckm::vec3 _position;
        ckm::quat _orient;
        
        Entity _parent;
        Entity _firstChild;
        Entity _nextSibling;
        
        bool _isDirty;
    };
}

} /* namespace overview */ } /* namespace cinek */


#endif
