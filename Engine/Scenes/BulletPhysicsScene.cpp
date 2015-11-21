//
//  BulletPhysicsScene.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#include "BulletPhysicsScene.hpp"

#include "bullet/btBulletDynamicsCommon.h"

namespace cinek {
    namespace ove {
    
BulletPhysicsScene::BulletPhysicsScene() :
    _btCollisionDispatcher(&_btCollisionConfig),
    _btWorld(&_btCollisionDispatcher,
             &_btBroadphase,
             &_btConstraintSolver,
             &_btCollisionConfig)
{
}

BulletPhysicsScene::~BulletPhysicsScene()
{
}
    
void BulletPhysicsScene::simulate(double dt)
{
    _btWorld.stepSimulation(dt);
}
    
    }   /* namespace ove */
}   /* namespace cinek */