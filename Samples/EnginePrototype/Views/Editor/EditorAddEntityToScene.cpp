//
//  EditorAddEntityToScene.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "EditorAddEntityToScene.hpp"

#include "Engine/Services/EntityService.hpp"
#include "Engine/Services/RenderService.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/AssetManifest.hpp"
#include "Engine/ViewStack.hpp"

#include "UICore/UIBuilder.hpp"

namespace cinek {


EditorAddEntityToScene::EditorAddEntityToScene
(
    GameViewContext* gameContext
) :
    GameState(gameContext),
    _stagedEntity(0),
    _displayTemplateSelector(false)

{
    _entityTemplateListboxState.highlightItem = -1;
    
    auto templateManifest = entityService().getDefinitions("entity");
    if (templateManifest && templateManifest->root().HasMember("entity")) {
        auto& templates = templateManifest->root()["entity"];
        
        _entityTemplateUIList.reserve(templates.MemberEnd() - templates.MemberBegin());
        
        for (auto templateIt = templates.MemberBegin();
             templateIt != templates.MemberEnd();
             ++templateIt)
        {
            addEntityTemplateUIData(templateIt->name.GetString(), templateIt->value);
        }
    }

}

void EditorAddEntityToScene::addEntityTemplateUIData
(
    std::string name,
    const JsonValue& entityTemplate
)
{
    EntityTemplateUIData data;
    data.name = std::move(name);
    
    auto editorTemplate = entityTemplate.FindMember("editor");
    if (editorTemplate != entityTemplate.MemberEnd()) {
        data.longname = editorTemplate->value["name"].GetString();
    }
    
    _entityTemplateUIList.emplace_back(std::move(data));
    if (_entityTemplateListboxState.highlightItem < 0) {
        _entityTemplateListboxState.highlightItem = 0;
    }
}

bool EditorAddEntityToScene::onUIDataItemRequest
(
    int id,
    uint32_t row,
    uint32_t col,
    uicore::DataObject& data
)
{
    if (id == kUIProviderId_EntityTemplates) {
        if (row < _entityTemplateUIList.size()) {
            auto& source = _entityTemplateUIList[row];
            if (col == 0) {
                data.type = uicore::DataObject::Type::string;
                data.data.str = source.longname.empty() ? source.name.c_str()
                                    : source.longname.c_str();
                return true;
            }
        }
    
    }
    
    return false;
}

uint32_t EditorAddEntityToScene::onUIDataItemRowCountRequest(int id)
{
    if (id == kUIProviderId_EntityTemplates) {
        return (uint32_t)_entityTemplateUIList.size();
    }
    return 0;
}


void EditorAddEntityToScene::onViewAdded(ove::ViewStack& stateController)
{
    _displayTemplateSelector = true;
}

void EditorAddEntityToScene::onViewRemoved(ove::ViewStack& stateController)
{
    _displayTemplateSelector = false;
    if (_stagedEntity) {
        entityService().destroyEntity(_stagedEntity);
        _stagedEntity = 0;
    }
}

void EditorAddEntityToScene::onViewStartFrame(ove::ViewStack& stateController)
{
}

void EditorAddEntityToScene::simulateView(ove::ViewStack& stateController, double dt)
{
}

void EditorAddEntityToScene::frameUpdateView
(
    ove::ViewStack& stateController,
    double dt,
    const cinek::uicore::InputState& inputState
)
{
    //  layout UI
    uicore::Layout::Style style;
    style.mask = uicore::Layout::Style::has_margins;
    style.margins = { 8, 8, 8, 8 };
    
    uicore::Layout uiLayout;
    auto frameWidth = renderService().getViewRect().w;
    auto frameHeight = renderService().getViewRect().h;
    
    uiLayout.beginFrame(UI_BUTTON0_DOWN, &_sceneFrameState, nullptr, nullptr)
        .setSize(frameWidth, frameHeight);
    
    if (_displayTemplateSelector) {
        uiLayout.beginColumn()
            .setLayout(UI_RIGHT | UI_VCENTER)
            .setSize(frameWidth/5, frameHeight*3/4)
            .beginWindow()
                .listbox(this, kUIProviderId_EntityTemplates,
                    uicore::ListboxType::kList,
                    &_entityTemplateListboxState,
                    &style)
            .end()
        .end();
    }
    
    uiLayout.end();
    
    if (inputState.testKey(SDL_SCANCODE_ESCAPE)) {
        stateController.present("EditorMain");
        return;
    }

    auto& hitResult = sceneRayTestResult();
    
    if (!_stagedEntity || !hitResult)
        return;
    
    if (hitResult.body->entity != _stagedEntity) {
        if (!hitResult.normal.fuzzyZero()) {
            auto stagedBody = scene().findBody(_stagedEntity);
            if (stagedBody) {
                stagedBody->setPosition(hitResult.position, hitResult.normal);
            }
        }
    }

}

void EditorAddEntityToScene::onViewEndFrame(ove::ViewStack& stateController)
{
    if (_displayTemplateSelector) {
        if (_entityTemplateListboxState.selected()) {
            _displayTemplateSelector = false;
            _stagedEntity = entityService().createEntity(kEntityStore_Staging, "entity",
                _entityTemplateUIList[_entityTemplateListboxState.selectedItem].name);
        }
    }
    else if (_stagedEntity) {
        if (_sceneFrameState.evtType == UI_BUTTON0_DOWN) {
            entityService().cloneEntity(kEntityStore_Default, _stagedEntity);
            stateController.present("EditorMain");
        }
    }

}
    
const char* EditorAddEntityToScene::viewId() const
{
    return "EditorAddEntityToScene";
}


} /* namespace cinek */