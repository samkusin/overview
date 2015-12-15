//
//  GameTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/2/15.
//
//

#ifndef Prototype_GameTypes_hpp
#define Prototype_GameTypes_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/State.hpp"

namespace cinek {

class GameController;
class GameControllerContext;

using GameState = ove::State<GameControllerContext>;

}


#endif /* Overview_GameTypes_hpp */
