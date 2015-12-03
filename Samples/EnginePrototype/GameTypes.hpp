//
//  GameTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/2/15.
//
//

#ifndef Overview_GameTypes_hpp
#define Overview_GameTypes_hpp

#include "Engine/EngineTypes.hpp"

namespace cinek {
    namespace ove {
        using GameScene = Scene<BulletPhysicsScene>;
        using GameSceneService = SceneService<GameScene>;
        using GameViewAPI = ViewAPI<GameScene, GameSceneService>;
    }
}


#endif /* Overview_GameTypes_hpp */
