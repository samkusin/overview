//
//  SceneMotionState.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/6/15.
//
//

#include "SceneMotionState.hpp"
#include "CKGfx/Node.hpp"

#include <bx/fpumath.h>

namespace cinek {
    namespace ove {

SceneMotionState::SceneMotionState(gfx::NodeHandle gfxNode) :
    _gfxNode(gfxNode)
{
}
    
void SceneMotionState::getWorldTransform(btTransform& worldTrans ) const
{
    const gfx::Matrix4& mtx = _gfxNode->transform();
    
    //  verify there's no scale/shearing
    CK_ASSERT(bx::vec3Length(&mtx.comp[0]) < 1.01f);
    CK_ASSERT(bx::vec3Length(&mtx.comp[4]) < 1.01f);
    CK_ASSERT(bx::vec3Length(&mtx.comp[8]) < 1.01f);
    
    //  convert from column major (Overview) to row major (Bullet)
    btMatrix3x3& basis = worldTrans.getBasis();
    basis[0].setValue(mtx.comp[0], mtx.comp[4], mtx.comp[8]);
    basis[1].setValue(mtx.comp[1], mtx.comp[5], mtx.comp[9]);
    basis[2].setValue(mtx.comp[2], mtx.comp[6], mtx.comp[10]);
    
    btVector3& translate = worldTrans.getOrigin();
    translate.setValue(mtx.comp[12], mtx.comp[13], mtx.comp[14]);
}

void SceneMotionState::setWorldTransform(const btTransform& worldTrans)
{
   /*
   const btMatrix3x3& basis = worldTrans.getBasis();
   const btVector3& translate = worldTrans.getOrigin();
   */
   gfx::Matrix4& mtx = _gfxNode->transform();
   
   static_assert(sizeof(btScalar) == sizeof(gfx::Matrix4::value_type),
                 "Types must be the same to extract matrix from worldTrans");
    
   worldTrans.getOpenGLMatrix(mtx.comp);
   
   //TODO, mark node aabb is dirty for later aabb regeneration
}
 

    } /* namesapce ove */
} /* namespace cinek */