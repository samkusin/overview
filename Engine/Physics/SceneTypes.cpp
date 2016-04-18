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
  
btQuaternion& btFromCkm(btQuaternion& btq, const ckm::quat& q)
{
    btq.setValue(q.x, q.y, q.z, q.w);
    return btq;
}

btVector3& btFromCkm(btVector3& btv, const ckm::vector3& v)
{
    btv.setValue(v.comp[0], v.comp[1], v.comp[2]);
    return btv;
}

ckm::quat& ckmFromBt(ckm::quat& q, const btQuaternion& btq)
{
    q.set(btq.getX(), btq.getY(), btq.getZ(), btq.getW());
    
    return q;
}

ckm::vector3& ckmFromBt(ckm::vector3& v, const btVector3& btv)
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

void SceneBody::setTransform(const ckm::quat& basis, const ckm::vector3& pos)
{
    btTransform& t = this->btBody->getWorldTransform();
    btQuaternion btq;
    t.setRotation(btFromCkm(btq, basis));
    btFromCkm(t.getOrigin(), pos);
    
    //  updates the revision number, copy is trivial
    this->btBody->setWorldTransform(t);
    transformChanged = true;
}

void SceneBody::setTransformMatrix(const ckm::matrix4 &mtx)
{
    btTransform& t = this->btBody->getWorldTransform();
    t.getBasis().setValue(mtx[0], mtx[4], mtx[8],
                          mtx[1], mtx[5], mtx[9],
                          mtx[2], mtx[6], mtx[10]);
    t.getOrigin().setValue(mtx[12], mtx[13], mtx[14]);
    
    //  updates the revision number, copy is trivial
    this->btBody->setWorldTransform(t);

    if (this->motionState) {
        this->motionState->setWorldTransform(t);
    }
}

void SceneBody::setVelocity(const ckm::vector3 &linear, const ckm::vector3 &angular)
{
    btFromCkm(linearVelocity, linear);
    btFromCkm(angularVelocity, angular);
    
    velocityChanged = true;
}

void SceneBody::getTransform(ckm::quat& basis, ckm::vector3& pos) const
{
    const btTransform& t = this->btBody->getWorldTransform();
    btQuaternion btq;
    ckmFromBt(basis, t.getRotation());
    ckmFromBt(pos, t.getOrigin());
}

void SceneBody::getTransformMatrix(ckm::matrix4 &mtx) const
{
    const btTransform& t = this->btBody->getWorldTransform();
    const btMatrix3x3& basis = t.getBasis();

    auto& row0 = basis.getRow(0);
    auto& row1 = basis.getRow(1);
    auto& row2 = basis.getRow(2);
    
    mtx[0] = row0.x();
    mtx[1] = row1.x();
    mtx[2] = row2.x();
    mtx[3] = ckm::scalar(0);
    mtx[4] = row0.y();
    mtx[5] = row1.y();
    mtx[6] = row2.y();
    mtx[7] = ckm::scalar(0);
    mtx[8] = row0.z();
    mtx[9] = row1.z();
    mtx[10] = row2.z();
    mtx[11] = ckm::scalar(0);
    
    const btVector3& origin = t.getOrigin();
    mtx[12] = origin.x();
    mtx[13] = origin.y();
    mtx[14] = origin.z();
    mtx[15] = ckm::scalar(1);
}

ckm::vector3 SceneBody::getPosition() const
{
    ckm::vector3 pos;
    const btTransform& t = this->btBody->getWorldTransform();
    ckmFromBt(pos, t.getOrigin());
    return pos;
}
    
bool SceneBody::checkFlags(uint32_t flags) const
{
    return (categoryMask & flags) != 0;
}
  
ckm::AABB<ckm::vector3> SceneBody::calcAABB() const
{
    ckm::AABB<ckm::vector3> aabb;
    
    btVector3 min;
    btVector3 max;
    btBody->getCollisionShape()->getAabb(this->btBody->getWorldTransform(), min, max);
    ckmFromBt(aabb.min, min);
    ckmFromBt(aabb.max, max);
    
    return aabb;
}

  
    } /* namespace ove */
} /* namespace cinek */
