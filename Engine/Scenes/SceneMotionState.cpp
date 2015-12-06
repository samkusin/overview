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
    
    CK_ASSERT(bx::vec3Length(&mtx.comp[0]) < 1.01f);
    CK_ASSERT(bx::vec3Length(&mtx.comp[4]) < 1.01f);
    CK_ASSERT(bx::vec3Length(&mtx.comp[8]) < 1.01f);
    
    btMatrix3x3& basis = worldTrans.getBasis();
    btVector3& translate = worldTrans.getOrigin();
    
    basis[0].setValue(mtx.comp[0], mtx.comp[1], mtx.comp[2]);
    basis[1].setValue(mtx.comp[4], mtx.comp[5], mtx.comp[6]);
    basis[2].setValue(mtx.comp[8], mtx.comp[9], mtx.comp[10]);
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
}
 

    } /* namesapce ove */
} /* namespace cinek */