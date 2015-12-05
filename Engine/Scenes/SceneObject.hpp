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
    
    int refCnt() const { return _refcnt; }
    
protected:
    SceneObject();
    uint8_t* buffer() { return _buffer; }
    
private:
    friend class SceneDataContext;
    friend class Scene;
    int incRef() { return ++_refcnt; }
    int decRef() { return --_refcnt; }
    
private:
    uint8_t _buffer[sizeof(ImplObject)];
    int16_t _refcnt;
    bool    _initialized;
};

template<typename _ImplObject>
SceneObject<_ImplObject>::SceneObject() :
    _refcnt(0),
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

class SceneTriangleMeshShape : public SceneObject<btBvhTriangleMeshShape>
{
public:
    SceneTriangleMeshShape(SceneFixedBodyHull* hull,
                           std::string name);
    
    const std::string& name() const { return _name; }
    
private:
    std::string _name;
};


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
