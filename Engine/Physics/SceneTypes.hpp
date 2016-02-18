//
//  SceneTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/6/16.
//
//

#ifndef Overview_SceneTypes_hpp
#define Overview_SceneTypes_hpp

#include "Engine/EngineTypes.hpp"

#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btMatrix3x3.h>
#include <bullet/LinearMath/btQuaternion.h>

class btTransform;
class btCollisionObject;
class btBvhTriangleMeshShape;

namespace cinek {
    namespace ove {
    
btMatrix3x3& btFromCkm(btMatrix3x3& btm, const ckm::matrix3f& m);
btVector3& btFromCkm(btVector3& btv, const ckm::vector3f& v);
ckm::matrix3f& ckmFromBt(ckm::matrix3f& m, const btMatrix3x3& btm);
ckm::vector3f& ckmFromBt(ckm::vector3f& v, const btVector3& btv);

struct SceneBody
{
    enum Category
    {
        kSection            = 0x01,
        kObject             = 0x02
    };

    bool isInCategory(Category c) const {
        return (categoryMask & c) != 0;
    }

    const SceneFixedBodyHull* getFixedHull() const;
    
    //  brute force setting of transform - TODO (refactor so that we conform to
    //  our set/getTranform rules outlined below.)
    void setPosition(const btVector3& pos, btVector3 up);
    
    //  set transform - the final transform may be modified by the Scene during
    //  update
    void setTransform(const ckm::matrix3f& basis, const ckm::vector3f& pos);
    //  returns the current world transform from the scene's perspective (i.e.
    //  not the final transform post update.)
    void getTransform(ckm::matrix3f& basis, ckm::vector3f& pos) const;

public:
    btCollisionObject* btBody = nullptr;
    SceneMotionState* motionState = nullptr;
    Entity entity = 0;
    
    enum
    {
        kIsSection          = 1 << kSection,
        kIsObject           = 1 << kObject,
        kAllCategories      = 0xffffffff
    };
    
    uint32_t categoryMask = 0;
};
   
struct SceneRayTestResult
{
    explicit operator bool() const { return body != nullptr; }
    
    SceneBody* body = nullptr;
    btVector3 normal;
    btVector3 position;
};
      
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneTypes_hpp */
