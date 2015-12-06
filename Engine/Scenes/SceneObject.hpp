//
//  SceneObject.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/4/15.
//
//

#ifndef Overview_SceneObject_hpp
#define Overview_SceneObject_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/Debug.hpp"

#include <bullet/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>

#include <string>

namespace cinek {
    namespace ove {

/**
 *  @class  SceneObject
 *  @brief  Wraps various Physics engine objects with information used by the
 *          controlling engine (Overview.)
 *
 *  The SceneObject attempts to replicate the ManagedObjectPool and
 *  ManagedHandle pattern, but wrapping pre C++1x objects (since Managed objects
 *  require move semantics.)
 */
template<typename _ImplObject>
class SceneObject
{
    CK_CLASS_NON_COPYABLE(SceneObject);
    
public:
    using ImplObject = _ImplObject;
    
    virtual ~SceneObject();
    
    ImplObject* impl() {
        return reinterpret_cast<ImplObject*>(buffer());
    }
    const ImplObject* impl() const {
        return reinterpret_cast<const ImplObject*>(buffer());
    }

protected:
    SceneObject();
    uint8_t* buffer() { return _buffer; }
    void setInitialized(bool b) { _initialized = b; }
    bool initialized() const { return _initialized; }
    
private:
    uint8_t _buffer[sizeof(ImplObject)];
    bool    _initialized;
};

template<typename _ImplObject>
SceneObject<_ImplObject>::SceneObject() :
    _initialized(false)
{
}

template<typename _ImplObject>
SceneObject<_ImplObject>::~SceneObject()
{
    if (_initialized) {
        impl()->~ImplObject();
    }
}

class SceneBody : public SceneObject<btRigidBody>
{
public:
    SceneBody(const btRigidBody::btRigidBodyConstructionInfo& info, Entity entity);
    
    Entity entity() const { return _entity; }
    
private:
    Entity _entity;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_SceneObject_hpp */
