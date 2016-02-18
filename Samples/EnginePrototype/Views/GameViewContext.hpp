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
#include "Engine/Physics/SceneTypes.hpp"
#include "UICore/Input.hpp"
#include "UICore/UITypes.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek {

class LoadSceneView;
class EditorView;
class PlayView;

struct GameViewContext
{
    gfx::Camera* camera;
    ove::SceneRayTestResult* screenRayTestResult;
    ove::SceneService* sceneService;
    ove::EntityService* entityService;
    ove::RenderService* renderService;
    ove::AssetService* assetService;
    UIService* uiService;
    NVGcontext* nvgContext;
    
    enum class Mode
    {
        kNone,
        kPlay,
        kEditor
    };
    
    void setMode(Mode mode) { _mode = mode; }
    Mode getMode() const { return _mode; }
    
private:
    Mode _mode;
};

} /* namespace cinek */


#endif /* Prototype_GameViewContext_hpp */
