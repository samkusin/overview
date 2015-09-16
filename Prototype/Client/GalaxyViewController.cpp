//
//  GalaxyViewController.cpp
//  Overview
//
//  Created by Samir Sinha on 7/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GalaxyViewController.hpp"

#include "CKGfx/ShaderLibrary.hpp"

#include "Engine/EngineMathGfx.hpp"

#include <glm/gtx/transform.hpp>

#include "Engine/Entity/TransformEntity.hpp"
#include "Engine/Render/Comp/Renderable.hpp"
#include "Custom/Comp/StellarSystem.hpp"
#include "Custom/Comp/StarBody.hpp"
#include "Custom/Comp/ComponentCreationCallback.hpp"

#include "UI/UIBuilder.hpp"
#include "CKGfx/External/nanovg/nanovg.h"

#include <cinek/entity/entitystore.hpp>

//  used for building a galactic "cell"
#include "Sim/BuildStarmap.hpp"
#include "Engine/BVH/AABBTree.hpp"

#include "Engine/Entity/Comp/Transform.hpp"

#include "Custom/Comp/StarBody.hpp"
#include "Custom/Comp/StellarSystem.hpp"
#include "Custom/Comp/RigidBody.hpp"

#include "Engine/Render/Comp/Renderable.hpp"
#include "Engine/Render/Comp/MeshRenderable.hpp"
#include "Engine/Render/Comp/Camera.hpp"

#include <cinek/debug.hpp>
#include <cinek/vector.hpp>

namespace cinek {

namespace ovproto {

struct MergeStarmapWithEntityStore
{
    bool operator()
    (
        BuildStarmapFunction::Result& starmap,
        EntityService entityService
    )
    {
        srcToDestEntityMap.clear();
        
        renderTable = entityService.table<overview::RenderableComponent>();
        
        //  copy transforms over to target store
        auto srcTransformTable = starmap.entityStore.table<overview::TransformComponent>();
        if (srcTransformTable)
        {
            //  update all transforms global positions
            overview::UpdateTransformComponent(srcTransformTable).all();
        
            FnData<overview::TransformComponent> data;
            data.utility = this;
            data.result = &starmap;
            data.entityService = entityService;
            data.target = entityService.table<overview::TransformComponent>();
            srcTransformTable.forEach
            (
                [&data](Entity eid, overview::TransformComponent& component)
                {
                    auto entity = data.getEntity(eid);
                    auto comp = data.target.addDataToEntity(entity);
                    
                    *comp = component;
                }
            );
        }
        
        auto srcSystemTable = starmap.entityStore.table<StellarSystemComponent>();
        if (srcSystemTable)
        {
            FnData<StellarSystemComponent> data;
            data.utility = this;
            data.result = &starmap;
            data.entityService = entityService;
            data.target = entityService.table<StellarSystemComponent>();
            srcSystemTable.forEach
            (
                [&data](Entity eid, StellarSystemComponent& component)
                {
                    auto entity = data.getEntity(eid);
                    auto comp = data.target.addDataToEntity(entity);
                    *comp = component;
        
                    //  set remapped entity for our tree node
                    data.result->stellarSystemTree.setNodeObjectId(comp->indexToTreeNode(), entity);
                    
                    data.utility->renderTable.addDataToEntity(entity);
                }
            );
        }
        
        auto srcStarBodyTable = starmap.entityStore.table<StarBodyComponent>();
        if (srcStarBodyTable)
        {
            FnData<StarBodyComponent> data;
            data.utility = this;
            data.result = &starmap;
            data.entityService = entityService;
            data.target = entityService.table<StarBodyComponent>();
            srcStarBodyTable.forEach
            (
                [&data](Entity eid, StarBodyComponent& component)
                {
                    auto entity = data.getEntity(eid);
                    auto comp = data.target.addDataToEntity(entity);
                    *comp = component;
                    
                    data.utility->renderTable.addDataToEntity(entity);
                }
            );
        }
        
        auto srcHierarchyTable = starmap.entityStore.table<overview::TransformComponent>();
        if (srcHierarchyTable)
        {
            //  rerun our iteration over the Transform.  child and sibling
            //  entities have already been created from above conversions.
            //  but we still need to fix the old child/sibling entities up.
            FnData<overview::TransformComponent> data;
            data.utility = this;
            data.result = &starmap;
            data.entityService = entityService;
            data.target = entityService.table<overview::TransformComponent>();
            srcHierarchyTable.forEach
            (
                [&data](Entity, overview::TransformComponent& component)
                {
                    component.setParent(data.getEntity(component.parent()));
                    component.setChild(data.getEntity(component.child()));
                    component.setSibling(data.getEntity(component.sibling()));
                }
            );
        }
        return true;
    }
    
private:
    template<typename _Component>
    struct FnData
    {
        MergeStarmapWithEntityStore* utility;
        BuildStarmapFunction::Result* result;
        EntityService entityService;
        component::Table<_Component> target;
        
        Entity getEntity(Entity srcEntity)
        {
            if (srcEntity==0)
                return srcEntity;
            
            auto res = utility->srcToDestEntityMap.emplace(srcEntity, Entity());
            if (res.second)
            {
                res.first->second = entityService.create();
            }
            return res.first->second;
        }
    };
    
    unordered_map<Entity, Entity> srcToDestEntityMap;
    
    component::Table<overview::RenderableComponent> renderTable;
};


struct Callback
{
    const EntityDataTable* table;
};
  

struct GalaxyStarmapRenderTree
{
    using RenderObjectList = vector<overview::RenderObject>;
    
    StellarSystemTree tree;
    RenderObjectList renderables;
    RenderObjectList nearRenderables;
    
    GalaxyStarmapRenderTree(const Allocator& allocator) : renderables(allocator)
    {
        renderables.reserve(32768);
        nearRenderables.reserve(4096);
    }
    
    const EntityDataTable* table;
    
    struct Callback
    {
        const EntityDataTable* renderables;

        GalaxyStarmapRenderTree* outputList;
        const ckm::Frustrum* frustrum;
        const ckm::Frustrum* nearFrustrum;
        
        Callback(const ckm::Frustrum& f, const ckm::Frustrum& nf,
                 EntityService store,
                 GalaxyStarmapRenderTree& output) :
            renderables(store.table<overview::RenderableComponent>().dataTable()),
            outputList(&output),
            frustrum(&f),
            nearFrustrum(&nf)
        {
        }
        
        bool operator()(Entity entity, const ckm::AABB<ckm::vec3>& aabb) const
        {
            overview::RenderObject obj;
            obj.renderableIdx = renderables->rowIndexFromEntity(entity);
            
            if (nearFrustrum->testAABB(aabb))
            {
                outputList->nearRenderables.push_back(obj);
            }
            else
            {
                outputList->renderables.push_back(obj);
            }
            return true;
        }
    };

    
    void buildObjectList
    (
        EntityService service,
        const ckm::Frustrum& frustrum,
        const ckm::Frustrum& nearFrustrum
    )
    {
        
        Callback cb(frustrum, nearFrustrum, service, *this);
        tree.test<StellarSystemTree::Test::FrustrumSweep>()(frustrum, cb);
    }
    
    void buildObjectListFromBox
    (
        const ckm::AABB<ckm::vec3>& box,
        vector<Entity>& entities
    )
    {
        struct Callback
        {
            vector<Entity>* entities;
            
            bool operator()(Entity entity, const ckm::AABB<ckm::vec3>& aabb) const
            {
                entities->push_back(entity);
                return true;
            }
        };
        
        Callback cb;
        cb.entities = &entities;
        
        tree.test<StellarSystemTree::Test::BoxSweep>()(box, cb);
    }
};

////////////////////////////////////////////////////////////////////////////////

PIDController::PIDController() :
    _inputTail(0),
    _output(0),
    _proportionalK(0),
    _integralK(0),
    _derivativeK(0),
    _dt(0)
{
}

PIDController::PIDController
(
    ckm::scalar proportionalK,
    ckm::scalar integralK,
    ckm::scalar derivativeK,
    ckm::scalar dt
) :
    _inputTail(0),
    _output(0),
    _proportionalK(proportionalK),
    _integralK(integralK),
    _derivativeK(derivativeK),
    _dt(dt)
{
}

void PIDController::resetSamples()
{
    _inputTail = 0;
    _output = 0;
}

void PIDController::addInput(ckm::scalar input)
{
    if (_inputTail == _inputs.size())
    {
        for (auto i = 1; i < _inputTail; ++i)
            _inputs[i-1] = _inputs[i];
    }
    else
    {
        ++_inputTail;
    }
    auto inputCur = _inputTail-1;
    _inputs[inputCur] = input;
    
    //  integration step requires a certain number of inputs
    //  so the turn will be delayed by this number of frames, which should be
    //  a tiny fraction of a second with reasonable time steps
    if (_inputTail < kReqInputs)
    {
        _output = 0;
        return;
    }
    
    //  Kp * in
    ckm::scalar proportional = _proportionalK * _inputs[inputCur];
    
    //  integral(t=0,inputCur) { Ki * in(t) }
    //  approximation using extended simpson's rule
    
    ckm::scalar integral = 0;
    for (auto i = 1; i < inputCur; i+=2)
    {
        integral += 4*_inputs[i];
    }
    for (auto i = 2; i < inputCur; i+=2)
    {
        integral += 2*_inputs[i];
    }
    integral += _inputs[0];
    integral += _inputs[inputCur];
    integral /= (3*_dt);
    integral *= _integralK;
    
    ckm::scalar derivative = _derivativeK * (_inputs[inputCur]-_inputs[inputCur-1]) / _dt;
    
    _output = 1 * (proportional + integral + derivative);
}


////////////////////////////////////////////////////////////////////////////////

GalaxyViewController::GalaxyViewController
(
    AppContext context,
    RenderContext renderContext
) :
    _API(context),
    _Render(renderContext),
    _Entity(context)
{
    _uniforms.fill(BGFX_INVALID_HANDLE);
    _starSystemBoxes.reserve(512);
}

GalaxyViewController::~GalaxyViewController()
{
}

void GalaxyViewController::onViewLoad()
{
    _API.createJsonDocumentFromFile(kDocumentId_EntityTemplates, "entity.json");
    
    //  position our new star entity - note, this is a brute force method that
    //  bypasses any notification, etc.  use a proper physics system or add
    //  a utility to manipulate entities that sends out the appropriate
    //  notification
    
    /*
    auto& entityMatrix = entityObj->matrix();
    bx::mtxSRT(entityMatrix, 2.0f, 2.0f, 2.0f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f);
    */
    BuildStarmapFunction cellGenFn;
    
    vector<SpectralClassDesc> spectralClasses =
    {
        {   55000, 275.0,    0.0,  0,  SpectralClassDesc::kWeibull  },
        {   30000,  16.0,   15.0,  5,  SpectralClassDesc::kWeibull  },
        {   10000,   2.1,   20.0,  5,  SpectralClassDesc::kUniform  },
        {   7500,    1.4,   25.0,  4,  SpectralClassDesc::kUniform  },
        {   6000,   1.04,   30.0,  3,  SpectralClassDesc::kUniform  },
        {   5200,    0.8,   35.0,  3,  SpectralClassDesc::kUniform  },
        {   3700,   0.45,   45.0,  2,  SpectralClassDesc::kUniform  },
        {   2400,   0.08,   50.0,  1,  SpectralClassDesc::kUniform  }
    };

    constexpr ckm::scalar kCellBoundZ = 32;
    constexpr ckm::scalar kCellBoundXY = 32;
    constexpr ckm::scalar kSolarMassTotal = 0.0016 * (kCellBoundXY*kCellBoundXY*kCellBoundZ*8);
    /*
    constexpr ckm::scalar kCellBoundZ = 4;
    constexpr ckm::scalar kCellBoundXY = 4;
    constexpr ckm::scalar kSolarMassTotal = 0.0008 * (kCellBoundXY*kCellBoundXY*kCellBoundZ*8);
    */
    vector<BuildStarmapFunction::SpectralInput> spectralInputs =
    {
        { 0.0 },
        { kSolarMassTotal * 0.001 },
        { kSolarMassTotal * 0.008 },
        { kSolarMassTotal * 0.010 },
        { kSolarMassTotal * 0.055 },
        { kSolarMassTotal * 0.076 },
        { kSolarMassTotal * 0.150 },
        { kSolarMassTotal * 0.700 }
    };
    
    ckm::vec3 cellMin( -kCellBoundXY,-kCellBoundXY,-kCellBoundZ );
    ckm::vec3 cellMax( kCellBoundXY, kCellBoundXY, kCellBoundZ );
    ckm::AABB<ckm::vec3> cellBounds(cellMin, cellMax);
    
    auto cell = cellGenFn(32768,
        spectralClasses,
        spectralInputs,
        7,
        cellBounds,
        0.1, 1.5);
    
    if (cell.result == BuildStarmapFunction::Result::kSuccess ||
        cell.result == BuildStarmapFunction::Result::kRegionFull)
    {
        //  merge results with our global entity store
        MergeStarmapWithEntityStore mergeFn;
        mergeFn(cell, _Entity);

        _starmap = allocate_unique<GalaxyStarmapRenderTree>(_allocator, _allocator);
        _starmap->tree = std::move(cell.stellarSystemTree);
    }
    else
    {
        return;
    }
    
    //  load shader with uniforms
    //  custom uniform definitions here
    _uniforms[kUniform_Scale] = bgfx::createUniform("u_scale", bgfx::UniformType::Vec4);
    _uniforms[kUniform_Texture0] = bgfx::createUniform("u_tex0", bgfx::UniformType::Int1);
    _uniforms[kUniform_Texture1] = bgfx::createUniform("u_tex1", bgfx::UniformType::Int1);
    
    _shaderProgram = _Render.renderResources().shaderLibrary->loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_RGBA_Tex0, kShaderIndex_Star),
        "shaders/vs_starmap.bin",
        "shaders/fs_starmap.bin");
    
    _starCorona = _Render.loadTexture("textures/starcorona.png");
    
    bx::mtxIdentity(_mapTransform);
    
    ckm::AABB<ckm::vec3> localBox(12);
    
    _starmapLocalEntities.reserve(512);
    _starmap->buildObjectListFromBox(localBox, _starmapLocalEntities);
    
    Entity currentSystem;
    currentSystem = 0;
    
    if (!_starmapLocalEntities.empty())
    {
        currentSystem = _starmapLocalEntities[std::rand() % _starmapLocalEntities.size()];
    }
    
    _driveTarget = 0;
    _driveTimer = 15;
    
    //  create camera
    _camera = _Entity.create("navi_camera");
    auto camera = _Entity.table<overview::CameraComponent>().dataForEntity(_camera);
    if (camera)
    {
        camera->init(0, M_PI * 90/180.0 , 1, 1000.0);
    }
    
    _systemCameraDist = 5.0;
    
    setCurrentSystem(currentSystem);
    
    _ship = _Entity.create("ship_gilgerard");
    
    //  starmap viewer
    _uiStarmapItem = -1;
    resetMouseTrack();
    
    //  Create player related entities
    //  Player Entity
    
    /*
    
    Player = PlayerUtility::createPlayer(_API, templateName)
             {
                Entity = EntityService::create(templateName);
                if 
                Entity.Loadout =
             }
    
    PlayerList::RoleLimits roleLimits;
    roleLimits.fill(0);
    roleLimits[PlayerRole::kCaptain] = 1;
    roleLimits[PlayerRole::kPilot] = 1;
    roleLimits[PlayerRole::kNavigator] = 1;
    roleLimits[PlayerRole::kEngineer] = 1;
    
    Party = PlayerListService::createList(roleLimits);
    
    Ship = EntityService::create(templateName);
    
    Player.Character.PlayerList = Party;
    Player.Loadout.Ship = Ship;
    
    //bx::mtxScale(mtx, 2.0f, 2.0f, 2.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    //bx::mtxTranslate(mtx, 0.0f, 0.0f, 0.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    
    entityObj = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    
    entityTransform = &entityObj->transform();
    bx::mtxRotateXYZ(entityTransform->matrix, 0.0f, 0.0f, bx::pi);
    bx::mtxTranslate(mtx, 2.f, 2.f, 0.f);
    bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    */
}

void GalaxyViewController::onViewUnload()
{
    _Entity.destroy(_ship);
    _Entity.destroy(_camera);
    
    _starmap = nullptr;
    
    overview::RenderResources& resources = _Render.renderResources();
    
    for (auto& uniform : _uniforms)
    {
        if (bgfx::isValid(uniform))
        {
            bgfx::destroyUniform(uniform);
        }
    }
    
    _uniforms.fill(BGFX_INVALID_HANDLE);
    
    if (_shaderProgram)
    {
        resources.shaderLibrary->unloadProgram(_shaderProgram);
        _shaderProgram = kNullHandle;
    }
    
    if (_starCorona)
    {
        _Render.unloadTexture(_starCorona);
        _starCorona = nullptr;
    }
}

void GalaxyViewController::onViewAdded()
{
}

void GalaxyViewController::onViewRemoved()
{
}

void GalaxyViewController::onViewForeground()
{
}

void GalaxyViewController::onViewBackground()
{
}

void GalaxyViewController::simulateView(double time, double dt)
{
    if (time >= _driveTimer)
    {
        if (!_driveTarget)
        {
            auto transforms = _Entity.table<overview::TransformComponent>();
            //  select new system
            auto systemTransform = transforms.dataForEntity(_currentSystem);
            ckm::AABB<ckm::vec3> localBox(16);
            localBox += systemTransform->localPosition();
            _starmapLocalEntities.clear();;
            _starmap->buildObjectListFromBox(localBox, _starmapLocalEntities);
        
            if (!_starmapLocalEntities.empty())
            {
                _driveTarget = _starmapLocalEntities[std::rand() % _starmapLocalEntities.size()];
            }
            _driveYaw = std::move(PIDController(1,0.05, 13, dt));
            _drivePitch = std::move(PIDController(1,0.05,13,dt));
        }
        else
        {
            
            if (_currentSystem != _driveTarget && _driveTarget)
            {
                auto transforms = _Entity.table<overview::TransformComponent>();
                auto camera = transforms.dataForEntity(_camera);
                auto target = transforms.dataForEntity(_driveTarget);
                auto rb = _Entity.table<RigidBodyComponent>().dataForEntity(_camera);
                
                //  Calculate the direction the camera should face.
                //  From there we 'drive' the camera from the camera's viewpoint to
                //  avoid having to use three PID controllers (for Z) and also for
                //  avoiding gimbal lock.
                //
                //  Based on this local orientation, we turn the camera in two
                //  directions (Left/Right - Y axis, or Up/Down - X axis)
                //
                auto mtxCameraOrient = ckm::mtx3x3FromQuat(camera->orient());
                auto mtxInvCameraOrient = ckm::inverse(mtxCameraOrient);
                //auto currentDir = mtxCameraOrient[2];
                auto targetDisplacement = target->position() - camera->position();
                auto targetDir = ckm::normalize(targetDisplacement);
                
                auto viewTargetDir = mtxInvCameraOrient * targetDir;
                
                
                //printf("CD: %.2lf,%.2lf,%.2lf, TD: %.2lf,%.2lf,%.2lf, VTD: %.2lf,%.2lf,%.2lf    ",
                //    currentDir.x, currentDir.y, currentDir.z,
                //    targetDirNorm.x,targetDirNorm.y,targetDirNorm.z,
                //    viewTargetDir.x, viewTargetDir.y, viewTargetDir.z);
                
                
                //  separate Y axis and X axis turn forces to simpify evaluation
                ckm::vec3 fwdAxis(0,0,1);
                //ckm::vec3 upAxis(0,1,0);
                
                auto yawDir = ckm::normalize(ckm::vec3(viewTargetDir.x, 0, viewTargetDir.z));
                ckm::scalar yawAngle = ckm::acos(ckm::dot(fwdAxis, yawDir));
                if (viewTargetDir.x < 0)
                    yawAngle = -yawAngle;

                auto pitchDir = ckm::normalize(ckm::vec3(0, viewTargetDir.y, viewTargetDir.z));
                ckm::scalar pitchAngle = ckm::acos(ckm::dot(fwdAxis, pitchDir));
                if (viewTargetDir.y < 0)
                    pitchAngle = -pitchAngle;
                
                // printf("Yaw: %.2lf, Pitch: %.2lf    ", yawAngle, pitchAngle);
                
                _drivePitch.addInput(pitchAngle);
                _driveYaw.addInput(yawAngle);
                
                auto pitchAngAcc =  -_drivePitch.output();
                auto yawAngAcc =  _driveYaw.output();
                
                // printf("Yaw Acc: %.2lf, Pitch Acc: %.2lf    ", yawAngAcc, pitchAngAcc);
            
                const ckm::scalar kMaxAngularAccel = 1 * ckm::pi();
                
                if (pitchAngAcc > kMaxAngularAccel)
                    pitchAngAcc = kMaxAngularAccel;
                else if (pitchAngAcc < -kMaxAngularAccel)
                    pitchAngAcc = -kMaxAngularAccel;
                
                if (yawAngAcc > kMaxAngularAccel)
                    yawAngAcc = kMaxAngularAccel;
                else if (yawAngAcc < -kMaxAngularAccel)
                    yawAngAcc = -kMaxAngularAccel;
                
                auto& inertia = rb->inertiaTensors();
                
                ckm::vec3 torque = pitchAngAcc * inertia[0];
                torque += yawAngAcc * inertia[1];
                
                auto torqueMag = ckm::vectorLength(torque);
                if (torqueMag > ckm::epsilon())
                {
                //    torque /= torqueMag;
                    torque = mtxCameraOrient * torque;
                }
                else
                {
                    torque = ckm::vec3(0);
                }
                
                //printf("T: %.2lf,%.2lf,%.2lf\n", torque.x,torque.y,torque.z);
                
                rb->setTorque(torque);
                
                auto& velocity = rb->velocity();
                auto distToTarget = ckm::vectorLength(targetDisplacement);
                auto angularSpeed = ckm::vectorLength(rb->angularVelocity());
                auto linearSpeed = ckm::vectorLength(velocity);
                
                if (std::abs(yawAngle) < 0.05 && std::abs(pitchAngle) < 0.05 &&
                    angularSpeed < 0.1 && distToTarget < 3 && linearSpeed < 0.1)
                {
                    _currentSystem = _driveTarget;
                    rb->stop();
                    rb->setForce(ckm::vec3(0,0,0));
                    rb->setTorque(ckm::vec3(0,0,0));
                    _systemCameraDist = distToTarget;
                }
                
                //  arrival steering behavior
                //
                //  target_offset = target - position
                //  distance = length (target_offset)
                //  ramped_speed = max_speed * (distance / slowing_distance)
                //  clipped_speed = minimum (ramped_speed, max_speed)
                //  desired_velocity = (clipped_speed / distance) * target_offset
                //  steering = desired_velocity - velocity


                
                if (distToTarget < 3.25)
                {
                    rb->setForce(-velocity);
                }
                else
                {
                    ckm::scalar kMaxSpeed = 2.0;
                    auto rampedSpeed = kMaxSpeed * (distToTarget/4.0);
                    auto clippedSpeed = std::min(rampedSpeed, kMaxSpeed);

                    auto desiredVelocity = (clippedSpeed/distToTarget) * targetDisplacement;
                    auto steering = desiredVelocity - velocity;
                    
                    rb->setForce(steering); // point mass, accel = dv/dt
                }
            }
            else
            {
                setCurrentSystem(_currentSystem);

                _driveTarget = 0;
                _driveTimer = time + 15;
            }
        }
    }
}

struct RenderSpaceCameraTranslate
{
    component::Table<overview::RenderableComponent> renderables;
    component::Table<overview::TransformComponent> transforms;
    ckm::vec4 offset;
    
    void onEntity(Entity e, overview::RenderableComponent& r)
    {
        auto transform = transforms.dataForEntity(e);
        if (transform)
        {
            ckm::mat4 srt;
            transform->calcMatrix(srt);
            srt[3] -= offset;
            ckm::convert(srt, r.worldSRT);
            
            //  iterate through children
            Entity child = transform->child();
            while (child)
            {
                auto childRenderable = renderables.dataForEntity(child);
                
                if (childRenderable)
                {
                    onEntity(child, *childRenderable);
                }
                auto childTransform = transforms.dataForEntity(child);
                child = childTransform->sibling();
            }
        }
    }
};

void GalaxyViewController::layoutView()
{
    auto viewRect = _Render.viewRect();

    //  Starmap data generation used to detect which system was selected by
    //  the user.  Due to the expense of frustrum culling, we'll generate our
    //  render list as well.  Since layoutView() is always tied to the render
    //  frame layout() could be considered a "prepare for rendering" step :
    //  linked to the rendering pipeline and detached from the simulation.
    //
    starmapLayout();

    //  Widget layout
    UILayout layout;
    
    layout.frame(viewUIRenderHook, this);
    _uiStarmapItem = layout.currentGroupItem();
    {
        layout.setEvents(UI_BUTTON0_DOWN, this, kID_VIEW).
            setSize(viewRect.w, viewRect.h);
            /*
            column(UI_FILL).
                buttonItem(UITHEME_ICON_GHOST, "Button A", this, kID_BUTTON).
                buttonItem(UITHEME_ICON_NEWFOLDER, "Button B", this, kID_BUTTON2).
                buttonItem(UITHEME_ICON_MONKEY, "Button C", this, kID_BUTTON3).
            end();
            */
    }
    layout.end();
}

void GalaxyViewController::onUIEvent
(
    int evtId,
    UIevent evtType,
    const UIeventdata& data
)
{
    if (evtId == kID_VIEW)
    {
        if (evtType == UI_BUTTON0_DOWN)
        {
            Entity hitEntity;
            hitEntity = 0;
            
            for (auto& box : _starSystemBoxes)
            {
                float r2x = data.cursor.x - box.x;
                float r2y = data.cursor.y - box.y;
                r2x *= r2x;
                r2y *= r2y;
                
                if (r2x + r2y < box.radius2)
                {
                    printf("Star Hit %d,%d = %" PRIu64 " at %d,%d\n",
                            data.cursor.x, data.cursor.y,
                            box.entity, box.x, box.y);
                    hitEntity = box.entity;
                    break;
                }
            }
            
            if (!hitEntity)
            {
                printf("View Hit!\n");
            }
        }
    }
    else if (evtId == kID_BUTTON)
    {
        printf("Button A Hit\n");
    }
    else if (evtId == kID_BUTTON2)
    {
        printf("Button B Hit\n");
    }
    else if (evtId == kID_BUTTON3)
    {
        printf("Button C Hit\n");
    }
    else
    {
        printf("UI Unknown Event\n");
    }
}

void GalaxyViewController::starmapLayout()
{
    auto viewRect = _Render.viewRect();
 
    //  calculate camera frustrum (view-space) and the view-to-world
    //  transformation used by object list providers to cull entities
    //  from world space

    auto transformTable = _Entity.table<overview::TransformComponent>();
    auto cameraTransform = transformTable.dataForEntity(_camera);
    auto camera = _Entity.table<overview::CameraComponent>().dataForEntity(_camera);
    
    ckm::mat4 cameraSRT;
    if (cameraTransform)
    {
        cameraTransform->calcMatrix(cameraSRT);
    }
    else
    {
        cameraSRT = ckm::mat4(1);
    }
    ckm::mat3 cameraBasis(cameraSRT);
        
    auto aspect = (ckm::scalar)viewRect.w / viewRect.h;
    
    //  used to cull all systems within the current region
    ckm::Frustrum worldFrustrum = ckm::Frustrum(camera->nearZ(),
                    camera->farZ(),
                    camera->fov(),
                    aspect
                ).transform(
                    cameraBasis,
                    ckm::vec3(cameraSRT[3])
                );
    
    //  used to cull systems within our viewable frustrum between 'near'
    //  and 'far', which use different rendering pipelines
    //  we run a test against the worldFrustrum, and use worldNearFrustrum
    //  to filter near objects.  this is more efficient than running two
    //  frustrum sweeps since we only need to traverse the starmap's entities
    //  once.
    ckm::Frustrum worldNearFrustrum = ckm::Frustrum(camera->nearZ(),
                    camera->nearZ() + 15.0,
                    camera->fov(),
                    aspect
                ).transform(
                    cameraBasis,
                    ckm::vec3(cameraSRT[3])
                );
    
    _starmap->renderables.clear();
    _starmap->nearRenderables.clear();
    _starmap->buildObjectList(_Entity, worldFrustrum, worldNearFrustrum);
    
    //  We translate renderable objects so that our render camera is at
    //  the origin.  Since its possible world objects use high precision
    //  coordinates, to avoid precision errors when dealing with render
    //  pipeline coordinates (always float) we translate coordinates
    //  into floats.  Of course this assumes we're not rendering objects
    //  at a distance beyond the float precision range (why would one
    //  do that?!)
    //
    //  Rotations and scale remain as they are in world space
    auto renderables = _Entity.table<overview::RenderableComponent>();
    auto systems = _Entity.table<StellarSystemComponent>();
    
    RenderSpaceCameraTranslate cameraTranslate;
    cameraTranslate.offset = cameraSRT[3];
    cameraTranslate.renderables = renderables;
    cameraTranslate.transforms = transformTable;

    _starSystemBoxes.clear();

    /*
    auto centerSystemRenderable = renderables.dataForEntity(_currentSystem);
    
    gfx::Vector3 posCenterSystem;
    
    if (centerSystemRenderable)
    {
        posCenterSystem.from(centerSystemRenderable->worldSRT[12],
            centerSystemRenderable->worldSRT[13],
            centerSystemRenderable->worldSRT[14]);
    }
    
    */
    gfx::Matrix4 mtxView;
    gfx::Matrix4 mtxProj;
    
    //  generate our bgfx view and projection matrices
    //  2d notes: if the camera is 2d, we create an orthogonal matrix
    //  general notes: near and far are assumed to be 'reasonable'
    //  values (meaning within float range)
    //
    bx::mtxInverse(mtxView, ckm::convert(cameraSRT, mtxView));
    bx::mtxProj(mtxProj,
        180.0f * (camera->fov()/M_PI), (float)aspect,
        (float)camera->nearZ(),
        (float)camera->farZ());
    
    gfx::Vector4 systemPos1;
    gfx::Vector4 systemPos2;
    gfx::Vector4 systemPosR1;
    gfx::Vector4 systemPosR2;
    
    for (auto& renderObject : _starmap->nearRenderables)
    {
        auto renderable = renderables.dataAtIndex(renderObject.renderableIdx);
        if (renderable.second)
        {
            cameraTranslate.onEntity(renderable.first, *renderable.second);
            
            //  create our UI spheres for each star system within clickable range
            auto system = systems.dataForEntity(renderable.first);
            auto adjSystemRadius = system->radiusInLYR() * 0.4;
            
            _starSystemBoxes.emplace_back();
            auto& systemBox = _starSystemBoxes.back();
            
            auto& worldSRT = renderable.second->worldSRT;
            systemPos1.from(worldSRT[12], worldSRT[13], worldSRT[14], worldSRT[15]);
            
            bx::vec4MulMtx(systemPos2, systemPos1, mtxView);
            systemPosR2.from(systemPos2.x() + adjSystemRadius, systemPos2.y(), systemPos2.z(), systemPos2.w());
            
            // use the view Z to accurately reflect Z depth for rendering UI
            systemBox.z = systemPos2.z();
            
            bx::vec4MulMtx(systemPos1, systemPos2, mtxProj);
            bx::vec3Mul(systemPos2, systemPos1, 1/systemPos1.w());
            
            systemBox.x = viewRect.x + viewRect.w * (systemPos2[0]+1)*0.5f;
            systemBox.y = viewRect.y - viewRect.h * (systemPos2[1]-1)*0.5f;
            
            bx::vec4MulMtx(systemPosR1, systemPosR2, mtxProj);
            bx::vec3Mul(systemPosR2, systemPosR1, 1/systemPosR1.w());
            
            systemBox.radius = viewRect.w * (systemPosR2[0] - systemPos2[0]);
            systemBox.radius2 = systemBox.radius * systemBox.radius;
            systemBox.entity = renderable.first;
        
            /*
            if (centerSystemRenderable)
            {
                vecFromCenter[0] = renderable.second->worldSRT[12] - posCenterSystem[0];
                vecFromCenter[1] = renderable.second->worldSRT[13] - posCenterSystem[1];
                vecFromCenter[2] = renderable.second->worldSRT[14] - posCenterSystem[2];
                
                if (bx::vec3Length(vecFromCenter) < 10.0f)
                {
                
                }
            }*/
        }
    }
    
    std::sort(_starSystemBoxes.begin(), _starSystemBoxes.end(),
        [](const StarSystemUIBox& l, const StarSystemUIBox& r) -> bool {
            return l.z < r.z;
        });
    
    for (auto& renderObject : _starmap->renderables)
    {
        auto renderable = renderables.dataAtIndex(renderObject.renderableIdx);
        if (renderable.second)
        {
            cameraTranslate.onEntity(renderable.first, *renderable.second);
        }
    }
}

void GalaxyViewController::viewUIRenderHook(void* context, NVGcontext* nvg)
{
    reinterpret_cast<GalaxyViewController*>(context)->viewUIRender(nvg);
}

void GalaxyViewController::viewUIRender(NVGcontext* nvg)
{
    if (!_starSystemBoxes.empty())
    {
        NVGcolor selectableColor = { 0.25f, 0.25f, 0.25f, 0.50f };
   
        auto zMin = _starSystemBoxes[0].z;
        auto zRange = _starSystemBoxes[_starSystemBoxes.size()-1].z - zMin;
        
        for (auto& box : _starSystemBoxes)
        {
            float alphaBasis = 0.0f;
            if (box.entity == _currentSystem)
                alphaBasis = 0.50f;
            
            {
                nvgBeginPath(nvg);
                nvgCircle(nvg, box.x, box.y, box.radius);
                
                selectableColor.a = 0.05f + alphaBasis * (1 - (box.z - zMin) / zRange);
                
                nvgStrokeColor(nvg, selectableColor);
                nvgStroke(nvg);
                nvgFillColor(nvg, selectableColor);
                nvgFill(nvg);
            }
        }
    }
}

void GalaxyViewController::frameUpdateView(double dt)
{
    auto viewRect = _Render.viewRect();
    auto aspect = (ckm::scalar)viewRect.w / viewRect.h;
    
    if (!_shaderProgram)
        return;
    
    bgfx::setViewRect(0, viewRect.x, viewRect.y, viewRect.w, viewRect.h);
    bgfx::submit(0);
            
    //  acquire camera transform, and use its rotation and scale to generate
    //  our render view matrix.
    auto transformTable = _Entity.table<overview::TransformComponent>();
    auto cameraTransform = transformTable.dataForEntity(_camera);
    auto camera = _Entity.table<overview::CameraComponent>().dataForEntity(_camera);
    
    ckm::mat4 cameraSRT;
    if (cameraTransform)
    {
        cameraTransform->calcMatrix(cameraSRT);
    }
    else
    {
        cameraSRT = ckm::mat4(1);
    }
    
    //  use our renderables generated from the starmapLayout()
    auto renderables = _Entity.table<overview::RenderableComponent>();
    
    //  center our camera at 0,0,0 (since our render objects are now
    //  positioned relative to the origin.)
    cameraSRT[3].x = 0;
    cameraSRT[3].y = 0;
    cameraSRT[3].z = 0;
    
    gfx::Matrix4 mtxView;
    gfx::Matrix4 mtxProj;
    
    //  generate our bgfx view and projection matrices
    //  2d notes: if the camera is 2d, we create an orthogonal matrix
    //  general notes: near and far are assumed to be 'reasonable'
    //  values (meaning within float range)
    //
    bx::mtxInverse(mtxView, ckm::convert(cameraSRT, mtxView));
    bx::mtxProj(mtxProj,
        180.0f * (camera->fov()/M_PI), (float)aspect,
        (float)camera->nearZ(),
        (float)camera->farZ());
    
    bgfx::setViewTransform(0, mtxView, mtxProj);
    
    //  a frequently/regularly updated buffer
    //  each vertex is a star point
    //  our strategy is to draw the same vertex six times and generate quad
    //  vertex positions and UVs inside the vertex shader.
    //
    //  until geometry shader support is added, this is likely the most platform
    //  agnostic approach.
    //
 
    float scale[4] = { 1.0f, 1.0f, 0, 0 };

    struct Vertex
    {
        float x,y,z;        //  x,y,z in world
        uint32_t abgr;      //
        float u,v;          //  0 = abs mag
    };
    
    struct SystemVisitor : public overview::TransformComponentVisitor<SystemVisitor>
    {
        component::Table<overview::RenderableComponent> renderables;
        component::Table<StarBodyComponent> stars;
        Vertex* vertices;
        int starIndex;
        uint16_t* indices;
        
        SystemVisitor(component::Table<overview::TransformComponent> table) :
            TransformComponentVisitor(table)
        {
        }
        
        bool operator()(Entity e)
        {
            visitTree(e);
            return true;
        }
        
        bool visit(Entity e, overview::TransformComponent& t)
        {
            auto star = stars.dataForEntity(e);
            if (star)
            {
                int vertIndex = starIndex*4;
                auto renderable = renderables.dataForEntity(e);
                
                auto vert = vertices + vertIndex;
                
                //  set position
                vert->x = renderable->worldSRT.comp[12];
                vert->y = renderable->worldSRT.comp[13];
                vert->z = renderable->worldSRT.comp[14];
                
                //  color
                vert->abgr = star->abgrColor();
                
                //  set attributes
                vert->u = star->visualMagnitude();
                
                auto base = vert;
                
                for (vert = vert+3; vert > base; --vert)
                {
                    *vert = *base;
                }
                
                //  6 vertices per star quad - coords calculated in shader.
                auto ind = indices + starIndex*6;
                ind[0] = vertIndex;
                ind[1] = vertIndex+1;
                ind[2] = vertIndex+2;
                ind[3] = vertIndex;
                ind[4] = vertIndex+2;
                ind[5] = vertIndex+3;
                
                ++starIndex;
            }
            return true;
        }
    };
    
    auto& shaderLibrary = _Render.renderResources().shaderLibrary;
    
    bgfx::setUniform(_uniforms[kUniform_Scale], &scale);
    bgfx::TextureHandle texture0 = _Render.bgfxTextureFromHandle(_starCorona);
    bgfx::setTexture(0, _uniforms[kUniform_Texture0], texture0);
    
    bgfx::ProgramHandle bgfxProgram = shaderLibrary->program(_shaderProgram);
    bgfx::setProgram(bgfxProgram);
    
    auto systems = _Entity.table<StellarSystemComponent>();
    auto transforms = _Entity.table<overview::TransformComponent>();
    
    SystemVisitor systemVisitor(transforms);
    systemVisitor.renderables = renderables;
    systemVisitor.stars = _Entity.table<StarBodyComponent>();
    
    //  generate far star vertices, color table indices and absolute magnitudes
    auto systemsToRender = &_starmap->renderables;
    while (!systemsToRender->empty())
    {
        bgfx::TransientVertexBuffer starTVB;
        bgfx::TransientIndexBuffer starTIB;
        
        const int kStarLimit = 8192;
    
        const bgfx::VertexDecl& vertexDecl =
            gfx::VertexTypes::declaration(gfx::VertexTypes::kVec3_RGBA_Tex0);
        if (!bgfx::allocTransientBuffers(&starTVB, vertexDecl, kStarLimit*4,
                                    &starTIB, kStarLimit*6))
        {
            break;
        }
    
        systemVisitor.starIndex = 0;
        systemVisitor.vertices = reinterpret_cast<Vertex*>(starTVB.data);
        systemVisitor.indices = reinterpret_cast<uint16_t*>(starTIB.data);

        for (; !systemsToRender->empty() &&
               (systemVisitor.starIndex + kMaxStarsPerSystem) < kStarLimit; )
        {
            auto object = systemsToRender->back();
            systemsToRender->pop_back();
            
            auto renderable = renderables.dataAtIndex(object.renderableIdx);
            auto entity = renderable.first;
            
            auto system = systems.dataForEntity(entity);
            if (system)
            {
                systemVisitor(entity);
            }
        }
        
        
        int starCount = systemVisitor.starIndex;
        if (starCount)
        {
            bgfx::setTransform(_mapTransform);
         
            bgfx::setVertexBuffer(&starTVB, 0, starCount*4);
            bgfx::setIndexBuffer(&starTIB, 0, starCount*6);
            
            //  enable alpha blending and disable the depth test since we've
            //  sorted our stars by Z.
            bgfx::setState(
                  BGFX_STATE_RGB_WRITE
                | BGFX_STATE_ALPHA_WRITE
                | BGFX_STATE_MSAA
                | BGFX_STATE_BLEND_ALPHA
            );
                    
            bgfx::submit(0);
        }
    }
    
    //  generate near star vertices, color table indices and absolute magnitudes
    systemsToRender = &_starmap->nearRenderables;
    while (!systemsToRender->empty())
    {
        bgfx::TransientVertexBuffer starTVB;
        bgfx::TransientIndexBuffer starTIB;
        
        const int kStarLimit = 8192;
    
        const bgfx::VertexDecl& vertexDecl =
            gfx::VertexTypes::declaration(gfx::VertexTypes::kVec3_RGBA_Tex0);
        if (!bgfx::allocTransientBuffers(&starTVB, vertexDecl, kStarLimit*4,
                                    &starTIB, kStarLimit*6))
        {
            break;
        }
    
        systemVisitor.starIndex = 0;
        systemVisitor.vertices = reinterpret_cast<Vertex*>(starTVB.data);
        systemVisitor.indices = reinterpret_cast<uint16_t*>(starTIB.data);

        for (; !systemsToRender->empty() &&
               (systemVisitor.starIndex + kMaxStarsPerSystem) < kStarLimit; )
        {
            auto object = systemsToRender->back();
            systemsToRender->pop_back();
            
            auto renderable = renderables.dataAtIndex(object.renderableIdx);
            auto entity = renderable.first;
            
            auto system = systems.dataForEntity(entity);
            if (system)
            {
                systemVisitor(entity);
            }
        }
        
        
        int starCount = systemVisitor.starIndex;
        if (starCount)
        {
            bgfx::setTransform(_mapTransform);
         
            bgfx::setVertexBuffer(&starTVB, 0, starCount*4);
            bgfx::setIndexBuffer(&starTIB, 0, starCount*6);
            
            //  enable alpha blending and disable the depth test since we've
            //  sorted our stars by Z.
            bgfx::setState(
                  BGFX_STATE_RGB_WRITE
                | BGFX_STATE_ALPHA_WRITE
                | BGFX_STATE_MSAA
                | BGFX_STATE_BLEND_ALPHA
            );
                    
            bgfx::submit(0);
        }
    }
    
    
    //  handle input
    if (_API.uiActiveItem() < 0 && !_driveTarget)
    {
        //  Camera Control
        auto mouseState = _API.mouseState();
        auto keyState = _API.keyState();
        
        if (mouseState.wheelX || mouseState.wheelY)
        {
            printf("%d,%d\n", mouseState.wheelX, mouseState.wheelY);
        }
        
        auto mouseDelta = mouseTrackDelta(mouseState.x, mouseState.y);
        
        if (keyState.scankeys[SDL_SCANCODE_LCTRL])
        {
            auto mouseDeltaScalarX = (float)mouseDelta.x / viewRect.w;
            auto mouseDeltaScalarY = (float)mouseDelta.y / viewRect.h;
            
            rotateSystemCamera(ckm::vec3(0,1,0), ckm::pi() * mouseDeltaScalarX);
            rotateSystemCamera(ckm::vec3(1,0,0), ckm::pi() * mouseDeltaScalarY);
            
            updateMouseTrack(mouseState.x, mouseState.y);
        }
        else
        {
            resetMouseTrack();
            rotateSystemCamera(ckm::vec3(0,1,0), ckm::pi() * 0.0015);
            rotateSystemCamera(ckm::vec3(-1,0,0), ckm::pi() * 0.0000);
        }
        
        zoomSystemCamera(mouseState.wheelY);
    }
}



void GalaxyViewController::setCurrentSystem(Entity system)
{
    _currentSystem = system;
    
    auto transforms = _Entity.table<overview::TransformComponent>();
    auto cameraTransform = transforms.dataForEntity(_camera);
    auto systemTransform = transforms.dataForEntity(_currentSystem);
    
    overview::UpdateTransformComponent updateTransform(transforms);
    updateTransform(_camera);

    //  camera should face the current system
    ckm::vec3 cameraToSystemDir =
        ckm::normalize(systemTransform->position() - cameraTransform->position());
    
    ckm::mat3 cameraBasis;
    cameraBasis = cameraTransform->calcBasis(cameraBasis);
    
    //  TODO: up?
    _systemCameraRot = ckm::quatFromUnitVectors(cameraBasis[2], cameraToSystemDir);
    _systemCameraCenter = systemTransform->position();
    _systemCameraDist = std::min(3.0, _systemCameraDist);
    
    buildCameraTransform(*cameraTransform);
    updateTransform(_camera);
}

void GalaxyViewController::rotateSystemCamera
(
    ckm::vec3 axis,
    ckm::scalar radians
)
{
    //  TODO: up?
    auto qRot = ckm::quatFromAngleAndAxis(radians, axis);
    _systemCameraRot = ckm::normalize(_systemCameraRot * qRot); // local to world
 
    auto transforms = _Entity.table<overview::TransformComponent>();
    auto cameraTransform = transforms.dataForEntity(_camera);
    overview::UpdateTransformComponent updateTransform(transforms);
    buildCameraTransform(*cameraTransform);
    updateTransform(_camera);
}

void GalaxyViewController::zoomSystemCamera(ckm::scalar offset)
{
    if (offset > ckm::epsilon() || offset < -ckm::epsilon())
    {
        _systemCameraDist += offset/10;
        if (_systemCameraDist < 2)
            _systemCameraDist = 2;
        else if(_systemCameraDist > 10)
            _systemCameraDist = 10;
    }
    
    auto transforms = _Entity.table<overview::TransformComponent>();
    auto cameraTransform = transforms.dataForEntity(_camera);
    overview::UpdateTransformComponent updateTransform(transforms);
    buildCameraTransform(*cameraTransform);
    updateTransform(_camera);
}

void GalaxyViewController::buildCameraTransform(overview::TransformComponent& t)
{
    auto systemCameraMtx = ckm::mtx4x4FromQuat(_systemCameraRot);
    systemCameraMtx[3] = ckm::vec4(_systemCameraCenter, 1);

    ckm::vec4 cameraPos(0,0,-_systemCameraDist,1);
    cameraPos = systemCameraMtx * cameraPos;

    t.setLocalOrient(_systemCameraRot);
    t.setLocalPosition(ckm::vec3(cameraPos));
}


void GalaxyViewController::resetMouseTrack()
{
    _lastTrackMouseX = -1;
    _lastTrackMouseY = -1;
}

void GalaxyViewController::updateMouseTrack(int x, int y)
{
    _lastTrackMouseX = x;
    _lastTrackMouseY = y;
}

ckm::ivec2 GalaxyViewController::mouseTrackDelta(int x, int y) const
{
    if (_lastTrackMouseY < 0 || _lastTrackMouseX < 0)
    {
        return ckm::ivec2(0,0);
    }
    return ckm::ivec2(x - _lastTrackMouseX, y - _lastTrackMouseY);
}

} /* namespace ovproto */

} /* namespace cinek */

