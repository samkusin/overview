//
//  GameControllerContext.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/10/15.
//
//

#include "GameControllerContext.hpp"

namespace cinek {

GameControllerContext::GameControllerContext
(
    GameController& controller
) :
    _controller(&controller)
{
}

}
