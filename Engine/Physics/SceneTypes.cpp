//
//  SceneTypes.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/6/16.
//
//

#include "SceneTypes.hpp"
#include "SceneFixedBodyHull.hpp"
#include "SceneMotionState.hpp"

#include "Engine/Debug.hpp"

#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleMeshShape.h>

#include <ckm/aabb.hpp>
#include <cmath>

namespace cinek {
    namespace ove {
  
btMatrix3x3& btFromCkm(btMatrix3x3& btm, const ckm::matrix3f& m)
{
    //  to row major from column major
    const float* comp= m.comp;
    btm.setValue(comp[0], comp[3], comp[6],
                 comp[1], comp[4], comp[7],
                 comp[2], comp[5], comp[8]);
    return btm;
}

btVector3& btFromCkm(btVector3& btv, const ckm::vector3f& v)
{
    btv.setValue(v.comp[0], v.comp[1], v.comp[2]);
    return btv;
}

ckm::matrix3f& ckmFromBt(ckm::matrix3f& m, const btMatrix3x3& btm)
{
    float* comp = m.comp;
    const btVector3& v0 = btm.getRow(0);
    const btVector3& v1 = btm.getRow(1);
    const btVector3& v2 = btm.getRow(2);
    comp[0] = v0.x();
    comp[1] = v1.x();
    comp[2] = v2.x();
    comp[3] = v0.y();
    comp[4] = v1.y();
    comp[5] = v2.y();
    comp[6] = v0.z();
    comp[7] = v1.z();
    comp[8] = v2.z();
    
    return m;
}

ckm::vector3f& ckmFromBt(ckm::vector3f& v, const btVector3& btv)
{
    v.comp[0] = btv.x();
    v.comp[1] = btv.y();
    v.comp[2] = btv.z();
    return v;
}

btVector3 btFromGfx(const gfx::Vector3& v)
{
    return btVector3(v.x, v.y, v.z);
}

const SceneFixedBodyHull* SceneBody::getFixedHull() const
{
    const SceneFixedBodyHull* hull = nullptr;
    if (btBody) {
        const btCollisionShape* shape = this->btBody->getCollisionShape();
        if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE) {
            const btTriangleMeshShape* triMeshShape =
                static_cast<const btTriangleMeshShape*>(shape);
            hull = static_cast<const SceneFixedBodyHull*>(triMeshShape->getMeshInterface());
        }
    }
    return hull;
}

void SceneBody::setPosition
(
    const btVector3 &pos,
    btVector3 up
)
{
    //CK_ASSERT(this->btBody->isStaticOrKinematicObject());

    btVector3 forward(0,0,1);
    
    if (up.isZero()) {
        up.setValue(0,1,0);
    }
    
    //  determine forward vector based on our world up direction
    btScalar dirdot = std::abs(forward.dot(up));
    if (dirdot > btScalar(0.90)) {
        if (up.z() >=btScalar(0)) {
            forward = btVector3(0,-1,0);
        }
        else {
            forward = btVector3(0,1,0);
        }
    }

    btVector3 side = btCross(forward, up);
    side.normalize();
    
    forward = btCross(side, up);
    forward.normalize();
    
    btTransform transform;
    transform.getBasis().setValue(side.x(), side.y(), side.z(),
                                  up.x(), up.y(), up.z(),
                                  forward.x(), forward.y(), forward.z());
    transform.setOrigin(pos);
    
    /*
    printf("s(%.2f,%.2f,%.2f)\nu(%.2f,%.2f,%.2f)\nf(%.2f,%.2f,%.2f)\n\n",
    side.x(),side.y(), side.z(),
    up.x(), up.y(), up.z(),
    forward.x(),forward.y(),forward.z());
    */
    
    this->btBody->setWorldTransform(transform);
    if (this->motionState) {
        this->motionState->setWorldTransform(transform);
    }
}

void SceneBody::setTransform(const ckm::matrix3f& basis, const ckm::vector3f& pos, bool simulate)
{
    btTransform& t = this->btBody->getWorldTransform();
    // transpose to row major from column
    btFromCkm(t.getBasis(), basis);
    btFromCkm(t.getOrigin(), pos);
    
    //  updates the revision number, copy is trivial
    this->btBody->setWorldTransform(t);
    
    transformChanged = simulate;
    if (!simulate) {
        if (this->motionState) {
            this->motionState->setWorldTransform(t);
        }
    }
}

void SceneBody::setVelocity(const ckm::vector3f &linear, const ckm::vector3f &angular)
{
    btFromCkm(linearVelocity, linear);
    btFromCkm(angularVelocity, angular);
    
    velocityChanged = true;
}

void SceneBody::getTransform(ckm::matrix3f& basis, ckm::vector3f& pos) const
{
    const btTransform& t = this->btBody->getWorldTransform();
    ckmFromBt(basis, t.getBasis());
    ckmFromBt(pos, t.getOrigin());
}

ckm::vector3f SceneBody::getPosition() const
{
    ckm::vector3f pos;
    const btTransform& t = this->btBody->getWorldTransform();
    ckmFromBt(pos, t.getOrigin());
    return pos;
}
    
bool SceneBody::checkFlags(uint32_t flags) const
{
    return (categoryMask & flags) != 0;
}
  
ckm::AABB<ckm::vector3f> SceneBody::calcAABB() const
{
    ckm::AABB<ckm::vector3f> aabb;
    
    btVector3 min;
    btVector3 max;
    btBody->getCollisionShape()->getAabb(this->btBody->getWorldTransform(), min, max);
    ckmFromBt(aabb.min, min);
    ckmFromBt(aabb.max, max);
    
    return aabb;
}

  
    } /* namespace ove */
} /* namespace cinek */
