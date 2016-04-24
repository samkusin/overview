//
//  SceneTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/6/16.
//
//

#ifndef Overview_SceneTypes_hpp
#define Overview_SceneTypes_hpp

#include "Engine/Component.hpp"
#include "CKGfx/GfxTypes.hpp"

#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btMatrix3x3.h>
#include <bullet/LinearMath/btQuaternion.h>
#include <bullet/BulletCollision/BroadphaseCollision/btBroadphaseProxy.h>

class btTransform;
class btCollisionObject;
class btBvhTriangleMeshShape;

namespace cinek {
    namespace ove {
    
btMatrix3x3& btFromCkm(btMatrix3x3& btm, const ckm::matrix3& m);
btVector3& btFromCkm(btVector3& btv, const ckm::vector3& v);
ckm::matrix3& ckmFromBt(ckm::matrix3& m, const btMatrix3x3& btm);
ckm::vector3& ckmFromBt(ckm::vector3& v, const btVector3& btv);

btVector3 btFromGfx(const gfx::Vector3& v);

struct SceneBody
{
public:
    //  Component Properties
    OVERVIEW_COMPONENT_PROPERTY_POINT3(Position, "Position")
    OVERVIEW_COMPONENT_PROPERTY_QUATERNION(Rotation, "Rotation")
    OVERVIEW_COMPONENT_PROPERTY_POINT3(Velocity, "Velocity")
    OVERVIEW_COMPONENT_PROPERTY_POINT3(AngularVelocity, "Ang. Vel")
    
public:
    enum Category
    {
        kSection        = 0,
        kDynamic        = 1,
        kStaging        = 2,
        kNumCategories
    };

    enum Flags
    {
        kIsSection          = 1 << kSection,
        kIsDynamic          = 1 << kDynamic,
        kIsStaging          = 1 << kStaging,
        kAllCategories      = 0xffffffff
    };
        
    enum Filters
    {
        //  bullet filters
        kDefaultFilter      = btBroadphaseProxy::DefaultFilter,
        kStaticFilter       = btBroadphaseProxy::StaticFilter,
        
        //  custom filters
        kStagingFilter      = 256,
        
        //  all filters
        kAllFilter          = -1
    };

    bool checkFlags(uint32_t flags) const;
    uint32_t getCategoryMask() const { return categoryMask; }

    const SceneFixedBodyHull* getFixedHull() const;
    
    //  TODO - figure out a way to eliminate this method, using one the methods
    //  below instead.
    void setPosition(const btVector3& pos, btVector3 up);
    //  set transform - the final transform may be modified by the Scene during
    void setTransform(const ckm::quat& basis, const ckm::vector3& pos);
    //  brute force set of transform, bypassing simulation
    void setTransformMatrix(const ckm::matrix4& mtx);
    
    //  returns the current world transform from the scene's perspective (i.e.
    //  not the final transform post update.)
    void getTransform(ckm::quat& basis, ckm::vector3& pos) const;
    void getTransformMatrix(ckm::matrix4& mtx) const;
    
    ckm::AABB<ckm::vector3> calcAABB() const;

public:
    btCollisionObject* btBody = nullptr;
    SceneMotionState* motionState = nullptr;
    btVector3 linearVelocity;
    btVector3 angularVelocity;
    
    Entity entity = 0;
    ckm::scalar mass = 0;
    
    bool transformChanged = false;
    bool velocityChanged = false;
    
private:
    friend class Scene;
    uint32_t categoryMask = 0;
};
   
struct SceneRayTestResult
{
    explicit operator bool() const { return body != nullptr; }
    void clear() { body = nullptr; }
    
    SceneBody* body = nullptr;
    btVector3 normal;
    btVector3 position;
};
      
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneTypes_hpp */
