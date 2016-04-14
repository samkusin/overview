//
//  EditorView.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 1/15/16.
//
//

#include "EditorView.hpp"

#include "Engine/AssetManifest.hpp"
#include "Engine/Physics/Scene.hpp"
#include "Engine/Physics/SceneDataContext.hpp"
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
    GameState(gameContext),
    _activeEntity(0)
{
}

EditorView::~EditorView()
{

}
    
void EditorView::onViewAdded(ove::ViewStack& stateController)
{
    game().setGameMode(GameMode::kEditor);
    
    createUIData();
    
    setIdleState();
    
    //  reset camera
    cinek::gfx::Matrix4 cameraRotMtx;
    bx::mtxSRT(camera().worldMtx, 1,1,1,0,0,0,0,2,-12);

    scene().deactivate();
    navSystem().deactivate();
}

void EditorView::onViewRemoved(ove::ViewStack& stateController)
{
    destroyUIData();
    
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

    updateMainUI(_uiStatus, displaySize.x, displaySize.y);
    
    renderOverlay();
    
    _freeCameraController.handleCameraInput(camera(), inputState, dt);

    _vsm.runFrameUpdate(dt);
}

void EditorView::onViewEndFrame(ove::ViewStack& stateController)
{
    _vsm.endFrame();
    
    if (game().getGameMode() == GameMode::kPlay) {
        stateController.present("PlayView");
    }
    
    endFrameMainUI(_uiStatus);
}

const char* EditorView::viewId() const
{
    return "EditorView";
}


////////////////////////////////////////////////////////////////////////

void EditorView::createUIData()
{
    _entityNamespaces.clear();
    _entityNamespaces.reserve(16);
    _entityCategories.clear();
    _entityCategories.reserve(32);
    
    if (!entityService().isEntityValid(_activeEntity)) {
        _activeEntity = 0;
    }
    
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
    
    //  default UI status
    _uiStatus.cameraOption = UIStatus::kCamera_Perspective;
    _uiStatus.cameraDirection = UIStatus::kCameraDirection_Current;
    _uiTransformStatus.system = UITransformStatus::kSystem_Global;
    _uiTransformStatus.mode = UITransformStatus::Mode::kTransform_Location;
    _uiTransformStatus.option = UITransformStatus::kTransform_Free;
}

void EditorView::destroyUIData()
{
    _sceneTree = nullptr;
}

void EditorView::rebuildSceneTree(cinek::EditorView::SceneTree &tree)
{
    _sceneTree->reset();
    _sceneTree->addScene(scene());
}

void EditorView::updateMainUI(UIStatus& status, float width, float height)
{
    const ImVec2 kDesktopPad { 10, 10 };
    const ImVec2 kCameraDims { 120, 150 };
    const ImVec2 kToolboxDims { 120, height - kDesktopPad.y*4 - kCameraDims.y };
    const ImVec2 kToolboxPos { kDesktopPad.x, kDesktopPad.y };
    const ImVec2 kCameraPos { kDesktopPad.x, kDesktopPad.y + kToolboxDims.y + kDesktopPad.y };
    const ImGuiIO& io = ImGui::GetIO();
    
    ImGui::SetNextWindowPos(kCameraPos, ImGuiSetCond_FirstUseEver);
    if (ImGui::Begin("Camera", nullptr, kCameraDims, 0.3f,
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::RadioButton("Perspective", &status.cameraOption, UIStatus::kCamera_Perspective);
        ImGui::RadioButton("Orthogonal", &status.cameraOption, UIStatus::kCamera_Orthogonal);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4,4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1,1));
        if (ImGui::Button("XYZ+")) {
            status.cameraDirection = UIStatus::kCameraDirection_XYZPos;
        }
        ImGui::SameLine();
        if (ImGui::Button("XZY+")) {
            status.cameraDirection = UIStatus::kCameraDirection_XZYPos;
        }
        ImGui::SameLine();
        if (ImGui::Button("YZX+")) {
            status.cameraDirection = UIStatus::kCameraDirection_YZXPos;
        }
        
        if (ImGui::Button("XYZ-")) {
            status.cameraDirection = UIStatus::kCameraDirection_XYZNeg;
        }
        ImGui::SameLine();
        if (ImGui::Button("XZY-")) {
            status.cameraDirection = UIStatus::kCameraDirection_XZYNeg;
        }
        ImGui::SameLine();
        if (ImGui::Button("YZX-")) {
            status.cameraDirection = UIStatus::kCameraDirection_YZXNeg;
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }
    ImGui::End();
        
    
    if (!status.displayMainUI) {
        return;
    }
    const ImVec2 kSceneTreeDims { 240, 240 };
    const ImVec2 kSceneTreePos {
        width - kDesktopPad.x - kSceneTreeDims.x,
        kDesktopPad.y
    };
    const ImVec2 kPropsDims { 240, height - kSceneTreeDims.y - kDesktopPad.y*4 };
    const ImVec2 kPropsPos {
        kSceneTreePos.x,
        kSceneTreePos.y + kSceneTreeDims.y + kDesktopPad.y
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
    
    //  handle events from the frame window
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        auto& sceneHitResult = sceneRayTestResult();
        
        if (ImGui::IsMouseClicked(0)) {
            if (sceneHitResult) {
                //  set the active entity (entity must be selectable)
                if (!sceneHitResult.body->checkFlags(ove::SceneBody::kIsSection)) {
                    _activeEntity = sceneHitResult.body->entity;
                }
            }
        }
    
    }
    
    //  handle shortcuts
    if (io.KeyShift) {
        if (ImGui::IsKeyPressed(SDLK_TAB)) {
            game().setGameMode(GameMode::kPlay);
        }
    }
    else {
        if (_activeEntity) {
            if (ImGui::IsKeyPressed(SDLK_t)) {
                setTransformEntityState(UITransformStatus::Mode::kTransform_Location);
            }
            else if (ImGui::IsKeyPressed(SDLK_r)) {
                setTransformEntityState(UITransformStatus::Mode::kTransform_Orientation);
            }
        }
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

void EditorView::renderOverlay()
{
    auto nvg = GameState::nvgContext();
    auto& camera = GameState::camera();
    
    nvgSave(nvg);

    const ove::SceneBody* body = scene().findBody(_activeEntity);
    
    if (_activeEntity) {
        //  entity cursor
        const float kHandleLength = 75.0f;
        const float kCircleRadius = 10.0f;
        
        ckm::vector3 ckpos = body->getPosition();
        gfx::Vector4 pos { ckpos.x, ckpos.y, ckpos.z, 1.0f };
        bool isPosOnscreen;
        gfx::Vector2 activeEntityScreenPos = camera.worldToScreenCoordinates(pos, &isPosOnscreen);
    
        if (isPosOnscreen) {
            //  center point
            nvgBeginPath(nvg);
            nvgCircle(nvg, activeEntityScreenPos.x, activeEntityScreenPos.y, kCircleRadius);
            nvgStrokeColor(nvg, nvgRGBA(255,255,255,255));
            nvgStroke(nvg);
            
            //  handles
            gfx::Vector2 handleDir[3];
            
            pos.x = ckpos.x + 1.0f;
            handleDir[0] = camera.worldToScreenCoordinates(pos);
            ckm::sub(handleDir[0], handleDir[0], activeEntityScreenPos);
            ckm::normalize(handleDir[0], handleDir[0]);
            
            pos.x = ckpos.x;
            pos.y = ckpos.y + 1.0f;
            handleDir[1] = camera.worldToScreenCoordinates(pos);
            ckm::sub(handleDir[1], handleDir[1], activeEntityScreenPos);
            ckm::normalize(handleDir[1], handleDir[1]);
            
            pos.y = ckpos.y;
            pos.z = ckpos.z + 1.0f;
            handleDir[2] = camera.worldToScreenCoordinates(pos);
            ckm::sub(handleDir[2], handleDir[2], activeEntityScreenPos);
            ckm::normalize(handleDir[2], handleDir[2]);
            
            gfx::Vector2 handleEnd;
            gfx::Vector2 handleStart;
            gfx::Vector2 handleOffset;
            
            //  X handle
            ckm::scale(handleStart, handleDir[0], kCircleRadius);
            ckm::add(handleStart, handleStart, activeEntityScreenPos);
            ckm::scale(handleEnd, handleDir[0], kHandleLength);
            ckm::add(handleEnd, handleEnd, activeEntityScreenPos);
            
            nvgBeginPath(nvg);
            nvgMoveTo(nvg, handleStart.x, handleStart.y);
            nvgLineTo(nvg, handleEnd.x, handleEnd.y);
            nvgStrokeColor(nvg, nvgRGBA(255,0,0,255));
            nvgStroke(nvg);
            
            //  Y Handle
            ckm::scale(handleStart, handleDir[1], kCircleRadius);
            ckm::add(handleStart, handleStart, activeEntityScreenPos);
            ckm::scale(handleEnd, handleDir[1], kHandleLength);
            ckm::add(handleEnd, handleEnd, activeEntityScreenPos);
            
            nvgBeginPath(nvg);
            nvgMoveTo(nvg, handleStart.x, handleStart.y);
            nvgLineTo(nvg, handleEnd.x, handleEnd.y);
            nvgStrokeColor(nvg, nvgRGBA(0,255,0,255));
            nvgStroke(nvg);
        
            //  Z Handle
            ckm::scale(handleStart, handleDir[2], kCircleRadius);
            ckm::add(handleStart, handleStart, activeEntityScreenPos);
            ckm::scale(handleEnd, handleDir[2], kHandleLength);
            ckm::add(handleEnd, handleEnd, activeEntityScreenPos);
            
            nvgBeginPath(nvg);
            nvgMoveTo(nvg, handleStart.x, handleStart.y);
            nvgLineTo(nvg, handleEnd.x, handleEnd.y);
            nvgStrokeColor(nvg, nvgRGBA(0,0,255,255));
            nvgStroke(nvg);
        }
    }
    
    nvgRestore(nvg);
}

void EditorView::endFrameMainUI(UIStatus &status)
{
    if (camera().mode() == gfx::Camera::Mode::kPerspective &&
        status.cameraOption == UIStatus::kCamera_Orthogonal) {
        camera().setMode(gfx::Camera::Mode::kOrthogonal);
    }
    else if (camera().mode() == gfx::Camera::Mode::kOrthogonal &&
        status.cameraOption == UIStatus::kCamera_Perspective) {
        camera().setMode(gfx::Camera::Mode::kPerspective);
    }
    
    if (status.cameraDirection != UIStatus::kCameraDirection_Current) {
        const ove::SceneBody* body = scene().findBody(_activeEntity);
        if (body) {
            ckm::vector3 fwd;
            ckm::vector3 up;
            ckm::vector3 side;
            
            float* mtx = camera().worldMtx.comp;
            
            switch (status.cameraDirection) {
            case UIStatus::kCameraDirection_XYZPos:
                fwd.set(0,0,1);
                up.set(0,1,0);
                side.set(1,0,0);
                break;
            case UIStatus::kCameraDirection_XYZNeg:
                fwd.set(0,0,-1);
                up.set(0,1,0);
                side.set(-1,0,0);
                break;
            case UIStatus::kCameraDirection_XZYPos:
                fwd.set(0,1,0);
                up.set(0,0,-1);
                side.set(1,0,0);
                break;
            case UIStatus::kCameraDirection_XZYNeg:
                fwd.set(0,-1,0);
                up.set(0,0,1);
                side.set(1,0,0);
                break;
            case UIStatus::kCameraDirection_YZXPos:
                fwd.set(1,0,0);
                up.set(0,1,0);
                side.set(0,0,1);
                break;
            case UIStatus::kCameraDirection_YZXNeg:
                fwd.set(-1,0,0);
                up.set(0,1,0);
                side.set(0,0,-1);
                break;
            default:
                break;
            }
            
            const float kDist = 3.0f;
            auto aabb = body->calcAABB();
            ckm::vector3 bodyPos = aabb.center();
            ckm::vector3 cameraPos;
            ckm::scale(cameraPos, fwd, -kDist);
            ckm::add(cameraPos, cameraPos, bodyPos);
            
            memcpy(mtx, side.comp, sizeof(*mtx)*3);
            memcpy(mtx+4, up.comp, sizeof(*mtx)*3);
            memcpy(mtx+8, fwd.comp, sizeof(*mtx)*3);
            memcpy(mtx+12, cameraPos.comp, sizeof(*mtx)*3);
        }
        
        status.cameraDirection = UIStatus::kCameraDirection_Current;
    }
}

void EditorView::updateTransformUI
(
    UITransformStatus& status
)
{
    const ImVec2 kDesktopPad { 10, 10 };
    const ImVec2 kBoxDims { 120, 180 };
    
    ImVec2 anchor { 0,0 };
    
    ImGui::SetNextWindowPos({ kDesktopPad.x+anchor.x, kDesktopPad.y+anchor.y },
        ImGuiSetCond_FirstUseEver);
    
    if (ImGui::Begin(status.mode == UITransformStatus::Mode::kTransform_Location ? "Translate" : "Rotate",
                     nullptr, kBoxDims, 0.3f,
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings)) {
        
        ImGui::RadioButton("View", &status.option, UITransformStatus::kTransform_Free);
        if (status.mode == UITransformStatus::Mode::kTransform_Location) {
            ImGui::RadioButton("Snap", &status.option, UITransformStatus::kTransform_Snap);
        }
        ImGui::RadioButton("X", &status.option, UITransformStatus::kTransform_X);
        ImGui::RadioButton("Y", &status.option, UITransformStatus::kTransform_Y);
        ImGui::RadioButton("Z", &status.option, UITransformStatus::kTransform_Z);
    }
    ImGui::End();
    
    auto& io = ImGui::GetIO();
    if (io.KeyShift) {
        status.system = UITransformStatus::kSystem_Local;
    }
    else {
        status.system = UITransformStatus::kSystem_Global;
    }
    
    //  used for combining transform modes
    if (ImGui::IsKeyPressed(SDLK_n)) {
        if (status.mode == UITransformStatus::Mode::kTransform_Location) {
            status.option = UITransformStatus::kTransform_Snap;
        }
    }
    else if (ImGui::IsKeyPressed(SDLK_v)) {
        status.option = UITransformStatus::kTransform_Free;
    }
    else if (ImGui::IsKeyPressed(SDLK_x)) {
        status.option = UITransformStatus::kTransform_X;
    }
    else if (ImGui::IsKeyPressed(SDLK_y)) {
        status.option = UITransformStatus::kTransform_Y;
    }
    else if (ImGui::IsKeyPressed(SDLK_z)) {
        status.option = UITransformStatus::kTransform_Z;
    }
}

void EditorView::handleTransformUI(UITransformStatus& status)
{
    if (!_activeEntity)
        return;
    
    auto body = scene().findBody(_activeEntity, ove::SceneBody::kIsStaging);
    if (!body)
        return;
    
    if (status.option == UITransformStatus::kTransform_Snap) {
        auto& hitResult = sceneRayTestResult();
        if (hitResult && hitResult.body->entity != _activeEntity) {
            if (!hitResult.normal.fuzzyZero()) {
                if (status.mode == UITransformStatus::Mode::kTransform_Location) {
                    body->setPosition(hitResult.position, hitResult.normal);
                }
            }
        }
    }
    else {
        const ImGuiIO& io = ImGui::GetIO();

        //  obtain separate position and basis elements used for manipulation
        ckm::matrix4 bodyWorldMtx;
        body->getTransformMatrix(bodyWorldMtx);
        
        ckm::vector3 bodyWorldPos;
        ckm::translateFromMatrix(bodyWorldPos, bodyWorldMtx);
        
        ckm::vector3 bodyViewPos;
        ckm::mul(bodyViewPos, bodyWorldPos, camera().viewMtx);
        
        ckm::matrix4 viewToSpaceMtx;
        ckm::matrix4 workingSpaceMtx;
        ckm::matrix4 interMatrix;           // scratch

        switch (status.option) {
        case UITransformStatus::kTransform_Free:
            viewToSpaceMtx.makeIdentity();
            ckm::mul(workingSpaceMtx, bodyWorldMtx, camera().viewMtx);
            break;
        case UITransformStatus::kTransform_X:
        case UITransformStatus::kTransform_Y:
        case UITransformStatus::kTransform_Z:
            if (status.system == UITransformStatus::kSystem_Global) {
                viewToSpaceMtx = camera().worldMtx;
                workingSpaceMtx = bodyWorldMtx;
            }
            else if (status.system == UITransformStatus::kSystem_Local) {
                ckm::inverse(interMatrix, bodyWorldMtx);
                ckm::mul(viewToSpaceMtx, camera().worldMtx, interMatrix);
                workingSpaceMtx.makeIdentity();
            }
            break;
        default:
            CK_ASSERT(false);   // unhandled?
        }
     
        ckm::vector3 workingPos;
        ckm::translateFromMatrix(workingPos, workingSpaceMtx);
        workingSpaceMtx[12] = workingSpaceMtx[13] = workingSpaceMtx[14] = 0;
     
        ckm::vector3 mouseDelta = calcMouseDelta(viewToSpaceMtx, bodyViewPos.z,
                io.DisplaySize.x*0.5f, io.DisplaySize.y*0.5f,
                io.MouseDelta.x, io.MouseDelta.y);
        
        switch (status.mode) {
        case UITransformStatus::Mode::kTransform_Location:
            if (status.option == UITransformStatus::kTransform_X) {
                mouseDelta.y = mouseDelta.z = 0;
            }
            else if (status.option == UITransformStatus::kTransform_Y) {
                mouseDelta.x = mouseDelta.z = 0;
            }
            else if (status.option == UITransformStatus::kTransform_Z) {
                mouseDelta.x = mouseDelta.y = 0;
            }
            ckm::add(workingPos, workingPos, mouseDelta);
            break;
        case UITransformStatus::Mode::kTransform_Orientation: {
                float yaw = 1.0f, pitch = 1.0f, roll = 1.0f;
                if (status.option == UITransformStatus::kTransform_X) {
                    yaw = 0.0f;
                    roll = 0.0f;
                }
                else if (status.option == UITransformStatus::kTransform_Y) {
                    pitch = 0.0f;
                    roll = 0.0f;
                }
                else if (status.option == UITransformStatus::kTransform_Z) {
                    pitch = 0.0f;
                    yaw = 0.0f;
                }
                ckm::matrix4 rotMatrix;
                ckm::eulerToMatrix(rotMatrix,
                    (mouseDelta.z + mouseDelta.y) * pitch * ckm::kPi,
                    (mouseDelta.x - mouseDelta.z) * yaw * ckm::kPi,
                    (mouseDelta.x - mouseDelta.y) * roll * ckm::kPi
                );
                ckm::mul(interMatrix, workingSpaceMtx, rotMatrix);
                workingSpaceMtx = interMatrix;
            }
            break;
        }
        
        workingSpaceMtx[12] = workingPos[0];
        workingSpaceMtx[13] = workingPos[1];
        workingSpaceMtx[14] = workingPos[2];
        
        
        
        switch (status.option) {
        case UITransformStatus::kTransform_Free:
            ckm::mul(bodyWorldMtx, workingSpaceMtx, camera().worldMtx);
            break;
        case UITransformStatus::kTransform_X:
        case UITransformStatus::kTransform_Y:
        case UITransformStatus::kTransform_Z:
            if (status.system == UITransformStatus::kSystem_Local) {
                ckm::mul(interMatrix, workingSpaceMtx, bodyWorldMtx);
                bodyWorldMtx = interMatrix;
            }
            else if (status.system == UITransformStatus::kSystem_Global) {
                bodyWorldMtx = workingSpaceMtx;
            }
            break;
        default:
            CK_ASSERT(false);   // unhandled?
        }
            
        //  apply new transformation to body
        body->setTransformMatrix(bodyWorldMtx);
    }
}

ckm::vector3 EditorView::calcMouseDelta
(
    const ckm::matrix4& transform,
    float z,
    float halfw, float halfh,
    float dx, float dy
)
{
    ckm::vector3 delta;
    ckm::vector3 origin;
    ckm::vector3 temp;
    temp = camera().viewPositionFromScreenCoordinate(halfw, halfw, z);
    ckm::mul(origin, temp, transform);
    temp = camera().viewPositionFromScreenCoordinate(halfw+dx, halfw+dy, z);
    ckm::mul(delta, temp, transform);
    
    return ckm::sub(delta, delta, origin);;
}
    

void EditorView::renderTransformUI(UITransformStatus& status)
{
    const ove::SceneBody* body = scene().findBody(_activeEntity);
    
    if (!body)
        return;
    
    if (status.option < UITransformStatus::kTransform_X ||
        status.option > UITransformStatus::kTransform_Z) {
        return;
    }
    
    ckm::vector3 bodyPos = body->getPosition();
    
    auto& decl = gfx::VertexTypes::declaration(gfx::VertexTypes::kVPositionColor);
    if (!bgfx::checkAvailTransientVertexBuffer(32, decl))
        return;
    bgfx::TransientVertexBuffer linesTVB;
    bgfx::allocTransientVertexBuffer(
        &linesTVB,
        32,
        gfx::VertexTypes::declaration(gfx::VertexTypes::kVPositionColor));
    
    uint32_t vertCnt = 0;
    auto vert0 = reinterpret_cast<gfx::VertexTypes::PositionColor*>(linesTVB.data);
    auto vert1 = reinterpret_cast<gfx::VertexTypes::PositionColor*>(linesTVB.data)+1;
    
    vert0->x = bodyPos.x;
    vert0->y = bodyPos.y;
    vert0->z = bodyPos.z;
    vert1->x = bodyPos.x;
    vert1->y = bodyPos.y;
    vert1->z = bodyPos.z;
    
    if (status.option == UITransformStatus::kTransform_X) {
        vert0->x -= 1000.0f;
        vert0->abgr = 0xff0000ff;
        vert1->x += 1000.0f;
        vert1->abgr = vert0->abgr;
    }
    else if (status.option == UITransformStatus::kTransform_Y) {
        vert0->y -= 1000.0f;
        vert0->abgr = 0xff00ff00;
        vert1->y += 1000.0f;
        vert1->abgr = vert0->abgr;
    }
    else if (status.option == UITransformStatus::kTransform_Z) {
        vert0->z -= 1000.0f;
        vert0->abgr = 0xffff0000;
        vert1->z += 1000.0f;
        vert1->abgr = vert0->abgr;
    }
    
    vertCnt += 2;
    
    gfx::Matrix4 baseTransform = gfx::Matrix4::kIdentity;
    
    bgfx::setViewTransform(camera().viewIndex, camera().viewMtx, camera().projMtx);
    bgfx::setTransform(baseTransform);
    bgfx::setVertexBuffer(&linesTVB, 0, vertCnt);
    
    bgfx::setState( 0
      | BGFX_STATE_PT_LINES
      | BGFX_STATE_POINT_SIZE(2)
      | BGFX_STATE_RGB_WRITE
      | BGFX_STATE_MSAA
      | BGFX_STATE_CULL_CCW
    );
    
    auto& programs = *renderContext().programs;
    bgfx::submit(camera().viewIndex, programs[gfx::kNodeProgramColor]);
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
            }
            
            if (ImGui::IsKeyPressed(SDLK_ESCAPE)) {
                setIdleState();
            }
            else if (ImGui::IsKeyPressed(SDLK_RETURN) || ImGui::IsMouseClicked(0)) {
                entityService().cloneEntity(kEntityStore_Default, _stagedEntity);
                setIdleState();
            }
        };
    
    state.endFn =
        [this]() {
            entityService().destroyEntity(_stagedEntity);
            _uiStatus.displayMainUI = true;
        };
    
    _vsm.setNextState(std::move(state));
}

void EditorView::setTransformEntityState(UITransformStatus::Mode mode)
{
    ove::ViewStateLogic state;
    
    _uiTransformStatus.mode = mode;
    
    state.id = kStateId_TransformEntity;
    state.beginFn =
        [this]() {
            _uiStatus.displayMainUI = false;
            _uiTransformStatus.option = UITransformStatus::kTransform_Free;
            scene().addCategoryToBody(_activeEntity, ove::SceneBody::kStaging);
        };
    
    state.frameUpdateFn =
        [this](CKTimeDelta dt) {
            updateTransformUI(_uiTransformStatus);
            handleTransformUI(_uiTransformStatus);
            renderTransformUI(_uiTransformStatus);
            
            if (ImGui::IsKeyPressed(SDLK_ESCAPE) || ImGui::IsMouseClicked(0)) {
                setIdleState();
            }
            else if (ImGui::IsKeyPressed(SDLK_RETURN)) {
                setIdleState();
            }
        };
    
    state.endFn =
        [this]() {
            scene().removeCategoryFromBody(_activeEntity, ove::SceneBody::kStaging);
            _uiStatus.displayMainUI = true;
        };
    
    _vsm.setNextState(std::move(state));
}

} /* namespace cinek */
