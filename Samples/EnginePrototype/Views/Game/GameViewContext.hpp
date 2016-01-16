//
//  GameViewContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#ifndef Prototype_GameViewContext_hpp
#define Prototype_GameViewContext_hpp

#include "GameTypes.hpp"
#include "Engine/Scenes/SceneTypes.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek {

class EditorView;
class PlayView;

struct GameViewContext
{
    gfx::Camera* _camera;
    ove::SceneRayTestResult* _screenRayTestResult;
};

} /* namespace cinek */


#endif /* Prototype_GameViewContext_hpp */
