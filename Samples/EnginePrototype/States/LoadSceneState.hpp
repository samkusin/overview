//
//  LoadSceneState.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/21/15.
//
//

#ifndef Prototype_LoadSceneState_hpp
#define Prototype_LoadSceneState_hpp

#include "GameTypes.hpp"

namespace cinek {

class LoadSceneState : public GameState
{
public:
    virtual void onBegin(GameControllerContext& controller) override;
    virtual void onUpdate(GameControllerContext& controller, double dt) override;
    virtual void onFrameUpdate(GameControllerContext& controller, double dt) override;
    virtual void onEnd(GameControllerContext& controller) override;
};

}

#endif /* LoadSceneState_hpp */
