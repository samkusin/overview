//
//  BulletPhysicsScene.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/20/15.
//
//

#ifndef Overview_Bullet_Physics_Scene_hpp
#define Overview_Bullet_Physics_Scene_hpp

#include "bullet/btBulletDynamicsCommon.h"

class btDiscreteDynamicsWorld;

namespace cinek {
    namespace ove {
    
class BulletPhysicsScene
{
public:
    BulletPhysicsScene();
    ~BulletPhysicsScene();
    
    void simulate(double dt);
    
private:
    btDefaultCollisionConfiguration _btCollisionConfig;
    btCollisionDispatcher _btCollisionDispatcher;
    btDbvtBroadphase _btBroadphase;
    btSequentialImpulseConstraintSolver _btConstraintSolver;
    btDiscreteDynamicsWorld _btWorld;
};
    
    }   /* namespace ove */
}   /* namespace cinek */

#endif /* Overview_Bullet_Physics_Scene_hpp */
