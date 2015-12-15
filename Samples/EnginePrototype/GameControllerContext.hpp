//
//  GameControllerContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/10/15.
//
//

#ifndef Prototype_GameControllerContext_hpp
#define Prototype_GameControllerContext_hpp

#include "GameTypes.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek {

class GameControllerContext
{

public:
    GameControllerContext(GameController& controller);

private:
    GameController* _controller;
};

}

#endif /* Prototype_GameControllerContext_hpp */
