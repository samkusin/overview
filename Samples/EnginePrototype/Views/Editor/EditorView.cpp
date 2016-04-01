//
//  EditorView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "EditorView.hpp"
#include "EditorMain.hpp"

#include "Engine/AssetManifest.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Render/RenderContext.hpp"
#include "Engine/Controller/NavSystem.hpp"
#include "Engine/Services/EntityService.hpp"

#include "CKGfx/Light.hpp"
#include "CKGfx/ModelSet.hpp"
#include "CKGfx/Geometry.hpp"
#include "CKGfx/External/nanovg/nanovg.h"

#include "UICore/UI.hpp"

#include <cinek/memorystack.hpp>
#include <ckjson/json.hpp>
#include <ckm/math.hpp>

#include <bx/fpumath.h>
#include <bgfx/bgfx.h>

namespace cinek {

struct EditorView::SceneTreeNode
{
    enum class Type
    {
        kEntity,
        kGroup
    };
    const char* name;
    Type type;
    Entity entity;
    
    SceneTreeNode* parent;
    SceneTreeNode* nextSibling;
    SceneTreeNode* prevSibling;
    SceneTreeNode* firstChild;
};

class EditorView::SceneTree
{
public:
    using Node = SceneTreeNode;
    
    SceneTree(int nodeCount, const Allocator& allocator) :
        _nodeStack(nodeCount * sizeof(Node), allocator),
        _stringStack(nodeCount * 32),
        _root(nullptr)
    {
    }
    
    Node* root() { return _root; }
    const Node* root() const { return _root; }
    
    void reset()
    {
        _nodeStack.reset();
        _stringStack.reset();
        
        _root = allocateNode("Root", Node::Type::kGroup);
    }
    
    void addScene(const ove::Scene& scene)
    {
        Node* node = appendChild(_root, "Bodies", Node::Type::kGroup);
        if (node) {
            scene.iterateBodies(ove::SceneBody::kAllCategories,
                [this, node](ove::SceneBody* body, uint32_t mask) {
                    if (!body->checkFlags(ove::SceneBody::kIsStaging)) {
                        Node* child = appendChild(node, "Entity", Node::Type::kEntity);
                        if (child) {
                            child->entity = body->entity;
                        }
                    }
                });
        }
    }
    
private:
    MemoryStack _nodeStack;
    CStringStack _stringStack;
    Node* _root;
    
    Node* allocateNode(const char* name, Node::Type type)
    {
        Node* node = reinterpret_cast<Node*>(_nodeStack.allocate(sizeof(Node)));
        if (node) {
            memset(node, 0, sizeof(*node));
            
            node->name = _stringStack.create(name);
            node->type = type;
        }
        return node;
    }
    
    Node* appendChild(Node* parent, const char* name, Node::Type type)
    {
        Node* node = allocateNode(name, type);
        if (node) {
            if (parent->firstChild) {
                Node* oldTail = parent->firstChild->prevSibling;
                parent->firstChild->prevSibling = node;
                oldTail->nextSibling = node;
                node->prevSibling = oldTail;
            }
            else {
                parent->firstChild = node;
                node->prevSibling = node;   //  tail as our head's prev sibling
            }
            node->parent = parent;
        }
        return node;
    }
};

EditorView::EntityCategory::EntityCategory(int cnt, int maxstrlen) :
    strstack(cnt*maxstrlen*2),
    size(cnt)
{
    names.reserve(cnt);
}


////////////////////////////////////////////////////////////////////////////////

EditorView::EditorView
(
    GameViewContext* gameContext
) :
    GameState(gameContext)
{
}

EditorView::~EditorView()
{

}
    
void EditorView::onViewAdded(ove::ViewStack& stateController)
{
    game().setGameMode(GameMode::kEditor);
    
    initUIData();
    
    setIdleState();
    
    //  reset camera
    cinek::gfx::Matrix4 cameraRotMtx;
    bx::mtxRotateXYZ(cameraRotMtx, 0, 0, 0);
    _freeCameraController.setTransform({ 0,2,-12}, cameraRotMtx);

    scene().deactivate();
    navSystem().deactivate();
}

void EditorView::onViewRemoved(ove::ViewStack& stateController)
{
    game().setGameMode(GameMode::kNone);
}

void EditorView::onViewStartFrame(ove::ViewStack& stateController)
{
    _vsm.startFrame();
}

void EditorView::simulateView(ove::ViewStack& stateController, CKTimeDelta dt)
{
}

void EditorView::frameUpdateView
(
    ove::ViewStack& stateController,
    CKTimeDelta dt,
    const cinek::input::InputState& inputState
)
{
    auto displaySize = ImGui::GetIO().DisplaySize;
    
    updateUI(_uiStatus, displaySize.x, displaySize.y);
    
    _freeCameraController.handleCameraInput(camera(), inputState, dt);
    
    _vsm.runFrameUpdate(dt);
}

void EditorView::onViewEndFrame(ove::ViewStack& stateController)
{
    _vsm.endFrame();
    
    if (game().getGameMode() == GameMode::kPlay) {
        stateController.present("PlayView");
    }
}

const char* EditorView::viewId() const
{
    return "EditorView";
}


////////////////////////////////////////////////////////////////////////

void EditorView::initUIData()
{
    _entityNamespaces.clear();
    _entityNamespaces.reserve(16);
    _entityCategories.clear();
    _entityCategories.reserve(32);
    
    //  first pass, enumerate categories
    entityService().enumerateDefinitions(
        [this](const std::string& ns, const ove::AssetManifest& manifest) {
            auto& manifestRoot = manifest.root();
            if (manifestRoot.HasMember("entity")) {
                auto& templates = manifestRoot["entity"];
                
                for (auto templateIt = templates.MemberBegin();
                          templateIt != templates.MemberEnd();
                          ++templateIt) {
                    auto& templateBody = templateIt->value;
                    auto it = templateBody.FindMember("editor");
                    if (it != templateBody.MemberEnd()) {
                        if (it->value.HasMember("category")) {
                            const char* categoryName = it->value["category"].GetString();
                            auto categoryIt = std::lower_bound(
                                _entityCategories.begin(), _entityCategories.end(), categoryName,
                                [](const decltype(_entityCategories)::value_type& v, const char* name) -> bool {
                                    return v.first < name;
                                });
                            
                            if (categoryIt == _entityCategories.end() || categoryIt->first != categoryName) {
                                categoryIt = _entityCategories.emplace(categoryIt);
                                categoryIt->first = categoryName;
                            }
                            ++categoryIt->second.size;
                        }
                    }
                }
            }
        }
    );
    
    for (auto& entityCategory : _entityCategories) {
        entityCategory.second = std::move(EntityCategory(entityCategory.second.size, 32));
    }
    
    //  second pass, fill category template lists
    entityService().enumerateDefinitions(
        [this](const std::string& ns, const ove::AssetManifest& manifest) {
            auto& manifestRoot = manifest.root();
            if (manifestRoot.HasMember("entity")) {
                auto& templates = manifestRoot["entity"];
                
                auto nsIt = std::find(_entityNamespaces.begin(), _entityNamespaces.end(), ns);
                if (nsIt == _entityNamespaces.end()) {
                    nsIt = _entityNamespaces.emplace(nsIt, ns);
                }
                
                int nsIndex = (int)(nsIt - _entityNamespaces.begin());
                
                for (auto templateIt = templates.MemberBegin();
                          templateIt != templates.MemberEnd();
                          ++templateIt) {
                    auto& templateBody = templateIt->value;
                    auto it = templateBody.FindMember("editor");
                    if (it != templateBody.MemberEnd()) {
                        if (it->value.HasMember("category")) {
                            const char* categoryName = it->value["category"].GetString();
                            const char* longName = it->value["name"].GetString();
                            auto categoryIt = std::lower_bound(
                                _entityCategories.begin(), _entityCategories.end(), categoryName,
                                [](const decltype(_entityCategories)::value_type& v, const char* name) -> bool {
                                    return v.first < name;
                                });
                            
                            auto& category = categoryIt->second;
                            EntityTemplate tmp = {
                                nsIndex,
                                category.strstack.create(templateIt->name.GetString()),
                                category.strstack.create(longName)
                            };
                            category.names.emplace_back(tmp);
                        }
                    }
                }
            }
        }
    );
    
    //  scene tree building
    _sceneTree = allocate_unique<SceneTree>(_allocator, 1024, _allocator);
    rebuildSceneTree(*_sceneTree);
}

void EditorView::rebuildSceneTree(cinek::EditorView::SceneTree &tree)
{
    _sceneTree->reset();
    _sceneTree->addScene(scene());
}

void EditorView::updateUI(UIStatus& status, float width, float height)
{
    const ImVec2 kDesktopPad { 20, 20 };
    
    if (status.displayMainUI) {
        const ImVec2 kToolboxDims { 120, height - kDesktopPad.y*2 };
        const ImVec2 kToolboxPos { kDesktopPad.x, kDesktopPad.y };
        const ImVec2 kSceneTreeDims { 240, 240 };
        const ImVec2 kSceneTreePos {
            width - kDesktopPad.x - kSceneTreeDims.x,
            kDesktopPad.y
        };
        const ImVec2 kPropsDims { 240, 400 };
        const ImVec2 kPropsPos {
            kSceneTreePos.x,
            kSceneTreePos.y + kSceneTreeDims.y + 10
        };
            
        //  TOOLBOX UI
        ImGui::SetNextWindowPos(kToolboxPos, ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("Toolbox", nullptr, kToolboxDims, 0.3f,
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoSavedSettings)) {
            
            //  create scene entities UI
            if (ImGui::CollapsingHeader("Create", nullptr, false, true))
            {
                const ImGuiStyle& style = ImGui::GetStyle();
                
                //  create bodies popup UI
                for (auto& entityCategory : _entityCategories) {
                    char name[32];
                    strncpy(name, entityCategory.first.c_str(), sizeof(name));
                    name[sizeof(name)-1] = '\0';
                    
                    if (ImGui::Button(name, ImVec2(kToolboxDims.x - style.WindowPadding.x*2,0))) {
                        ImGui::OpenPopup(name);
                    }
                    if (ImGui::BeginPopup(name)) {
                        //  templates
                        for (auto& item : entityCategory.second.names) {
                            if (ImGui::Selectable(item.longName)) {
                                status.addEntityTemplate = item;
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
                
                ImGui::Dummy(ImVec2(0, ImGui::GetTextLineHeight()));
                
                //  create specialized entity UI
                if (ImGui::Button("Area", ImVec2(kToolboxDims.x - style.WindowPadding.x*2,0))) {
                
                }
                ImGui::Unindent();
            }
          
        }
        ImGui::End();
        
        //  SCENE GRAPH UI
        ImGui::SetNextWindowPos(kSceneTreePos, ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("Scene", nullptr, kSceneTreeDims, 0.3f)) {
            updateSceneTreeUI(_sceneTree->root());
        }
        ImGui::End();
        
        //  PROPERTIES UI
        ImGui::SetNextWindowPos(kPropsPos, ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("Properties", nullptr, kPropsDims, 0.3f)) {
        
        }
        ImGui::End();
    }
}

void EditorView::updateSceneTreeUI(const SceneTreeNode *node)
{
    if (!node)
        return;
    
    bool open = false;
    
    if (node->type == SceneTreeNode::Type::kGroup) {
        open = ImGui::TreeNode(node->name);
    }
    else if (node->type == SceneTreeNode::Type::kEntity) {
        auto& name = entityService().identityFromEntity(node->entity);
        const char* nameptr = "<Entity>";
        if (!name.empty()) {
            nameptr = name.c_str();
        }
        open = ImGui::TreeNode(node, "%s %" PRIx64 , nameptr, node->entity);
    }
    
    if (open) {

        for (const SceneTreeNode* child = node->firstChild;
                                  child;
                                  child = child->nextSibling) {
        
        
            updateSceneTreeUI(child);
        }
        ImGui::TreePop();
    }
}

void EditorView::setIdleState()
{
    ove::ViewStateLogic state;
    
    state.id = kStateId_Idle;
    state.beginFn =
        [this]() {
            _uiStatus.displayMainUI = true;
            _uiStatus.addEntityTemplate.reset();
            
            rebuildSceneTree(*_sceneTree);
        };
    
    state.frameUpdateFn =
        [this](CKTimeDelta dt) {
            if (_uiStatus.addEntityTemplate) {
                setAddEntityToSceneState();
            }
        };
    
    state.endFn =
        [this]() {
        };
    
    _vsm.setNextState(std::move(state));
}

void EditorView::setAddEntityToSceneState()
{
    ove::ViewStateLogic state;
    
    state.id = kStateId_AddEntityToScene;
    state.beginFn =
        [this]() {
            _uiStatus.displayMainUI = false;
           auto& ns = _entityNamespaces[_uiStatus.addEntityTemplate.nsIndex];
            _stagedEntity = entityService().createEntity(kEntityStore_Staging, ns, _uiStatus.addEntityTemplate.name);
        };
    
    state.frameUpdateFn =
        [this](CKTimeDelta dt) {
            auto& hitResult = sceneRayTestResult();
            if (hitResult) {
                if (hitResult.body->entity != _stagedEntity) {
                    if (!hitResult.normal.fuzzyZero()) {
                        auto stagedBody = scene().findBody(_stagedEntity);
                        if (stagedBody) {
                            stagedBody->setPosition(hitResult.position, hitResult.normal);
                        }
                    }
                }
                
                if (ImGui::IsKeyPressed(SDLK_ESCAPE)) {
                    entityService().destroyEntity(_stagedEntity);
                    setIdleState();
                }
                else if (ImGui::IsKeyPressed(SDLK_RETURN) || ImGui::IsMouseClicked(0)) {
                    entityService().cloneEntity(kEntityStore_Default, _stagedEntity);
                    setIdleState();
                }
            }
        };
    
    state.endFn =
        [this]() {
            _uiStatus.displayMainUI = true;
        };
    
    _vsm.setNextState(std::move(state));
}

} /* namespace cinek */
