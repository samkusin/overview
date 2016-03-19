//
//  EditorAddEntityToScene.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#ifndef Prototype_Game_Editor_AddEntityToScene_hpp
#define Prototype_Game_Editor_AddEntityToScene_hpp

#include "Engine/ViewController.hpp"

#include "GameViewContext.hpp"

#include <ckjson/jsontypes.hpp>
#include <vector>
#include <string>

namespace cinek {

class EditorAddEntityToScene : public GameState,
                               public uicore::DataProvider
{
public:
    EditorAddEntityToScene(GameViewContext* gameContext);
    
private:
    //  ViewController
    virtual void onViewAdded(ove::ViewStack& stateController) override;
    virtual void onViewRemoved(ove::ViewStack& stateController) override;
    virtual void onViewStartFrame(ove::ViewStack& stateController) override;
    virtual void simulateView(ove::ViewStack& stateController, CKTimeDelta dt) override;
    virtual void frameUpdateView(ove::ViewStack& stateController, CKTimeDelta dt,
                                 const cinek::uicore::InputState& inputState) override;
    virtual void onViewEndFrame(ove::ViewStack& stateController) override;
        
    virtual const char* viewId() const override;

    //  DataProvider
    virtual bool onUIDataItemRequest(int id, uint32_t row, uint32_t col,
        uicore::DataObject& data) override;
    virtual uint32_t onUIDataItemRowCountRequest(int id) override;

private:    
    uicore::ListboxState _entityTemplateListboxState;
    Entity _stagedEntity;

    bool _displayTemplateSelector;

private:
    enum
    {
        kUIProviderId_EntityTemplates
    };
    
    struct EntityTemplateUIData
    {
        std::string name;
        std::string longname;
    };
    std::vector<EntityTemplateUIData> _entityTemplateUIList;
    
    void addEntityTemplateUIData(std::string name, const JsonValue& entityTemplate);
};

} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
