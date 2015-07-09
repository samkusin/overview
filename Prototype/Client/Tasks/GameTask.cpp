//
//  GameTask.cpp
//  Overview
//
//  Created by Samir Sinha on 6/1/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GameTask.hpp"

#include "RenderInterface.hpp"
#include "Engine/Render/Renderer.hpp"

//  used for building a galactic "cell"
#include "Sim/BuildStarmap.hpp"
#include "Engine/BVH/AABBTree.hpp"

#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Custom/Comp/StarBody.hpp"
#include "Custom/Comp/StellarSystem.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"
#include "Engine/Entity/Comp/MeshRenderable.hpp"
#include "Engine/Entity/Comp/Camera.hpp"

#include <cinek/debug.hpp>
#include <cinek/vector.hpp>
#include <random>


namespace cinek {

using namespace overview;

namespace ovproto {


struct MergeStarmapWithEntityStore
{
    bool operator()
    (
        BuildStarmapFunction::Result& starmap,
        overview::EntityStore& destStore
    )
    {
        srcToDestEntityMap.clear();
        
        renderTable = destStore.table<overview::component::Renderable>();
        
        //  copy transforms over to target store
        auto srcTransformTable = starmap.entityStore.table<overview::component::Transform>();
        if (srcTransformTable)
        {
            FnData<overview::component::Transform> data;
            data.utility = this;
            data.result = &starmap;
            data.store = &destStore;
            data.target = destStore.table<overview::component::Transform>();
            srcTransformTable.forEach
            (
                [&data](Entity eid, overview::component::Transform& component)
                {
                    auto entity = data.getEntity(eid);
                    auto comp = data.target.addComponentToEntity(entity);
                    
                    *comp = component;
                }
            );
        }
        
        auto srcSystemTable = starmap.entityStore.table<component::StellarSystem>();
        if (srcSystemTable)
        {
            FnData<component::StellarSystem> data;
            data.utility = this;
            data.result = &starmap;
            data.store = &destStore;
            data.target = destStore.table<component::StellarSystem>();
            srcSystemTable.forEach
            (
                [&data](Entity eid, component::StellarSystem& component)
                {
                    auto entity = data.getEntity(eid);
                    auto comp = data.target.addComponentToEntity(entity);
                    *comp = component;
        
                    //  set remapped entity for our tree node
                    data.result->stellarSystemTree.setNodeObjectId(comp->indexToTreeNode, entity);
                    
                    data.utility->renderTable.addComponentToEntity(entity);
                }
            );
        }
        
        auto srcStarBodyTable = starmap.entityStore.table<component::StarBody>();
        if (srcStarBodyTable)
        {
            FnData<component::StarBody> data;
            data.utility = this;
            data.result = &starmap;
            data.store = &destStore;
            data.target = destStore.table<component::StarBody>();
            srcStarBodyTable.forEach
            (
                [&data](Entity eid, component::StarBody& component)
                {
                    auto entity = data.getEntity(eid);
                    auto comp = data.target.addComponentToEntity(entity);
                    *comp = component;
                    
                    data.utility->renderTable.addComponentToEntity(entity);
                }
            );
        }
        
        auto srcHierarchyTable = starmap.entityStore.table<overview::component::Transform>();
        if (srcHierarchyTable)
        {
            //  rerun our iteration over the Transform.  child and sibling
            //  entities have already been created from above conversions.
            //  but we still need to fix the old child/sibling entities up.
            FnData<overview::component::Transform> data;
            data.utility = this;
            data.result = &starmap;
            data.store = &destStore;
            data.target = destStore.table<overview::component::Transform>();
            srcHierarchyTable.forEach
            (
                [&data](Entity, overview::component::Transform& component)
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
        overview::EntityStore* store;
        component::Table<_Component> target;
        
        Entity getEntity(Entity srcEntity)
        {
            if (srcEntity==0)
                return srcEntity;
            
            auto res = utility->srcToDestEntityMap.emplace(srcEntity, Entity());
            if (res.second)
            {
                res.first->second = store->create();
            }
            return res.first->second;
        }
    };
    
    unordered_map<Entity, Entity> srcToDestEntityMap;
    
    component::Table<overview::component::Renderable> renderTable;
};


struct GameTask::Starmap
{
    StellarSystemTree tree;
    
    void buildObjectList
    (
        const overview::EntityStore& store,
        const ckm::Frustrum& frustrum,
        RenderObjectListWriter& writer
    )
    {
        struct Callback
        {
            const overview::component::EntityDataTable* renderables;
            RenderObjectListWriter* writer;
            
            Callback(const overview::EntityStore& store,
                     RenderObjectListWriter& writer) :
                renderables(store.table<overview::component::Renderable>().data()),
                writer(&writer)
            {
            }
            
            bool operator()(Entity entity) const
            {
                RenderObject obj;
                obj.renderableIdx = renderables->rowIndexFromEntity(entity);
                writer->insertObject(obj);
                return true;
            }
        };
        
        Callback cb(store, writer);
     
        RenderCommand::Params params;
        params.pipelineIdx = 0;
        
        writer.beginCommand(params);
        
        tree.test<StellarSystemTree::Test::FrustrumSweep>()(frustrum, cb);
        writer.endCommand();
    }
};


GameTask::GameTask(AppInterface api) :
    _API(api),
    _buildStarmapRenderHandler(kNullHandle)
{
}

GameTask::~GameTask()
{
}

void GameTask::onBegin()
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
    
    vector<SpectralClass> spectralClasses =
    {
        {   55000, 275.0,  38.0, 1000000,    0.0,  0,  SpectralClass::kWeibull  },
        {   30000,  16.0,   6.6,   30000,   15.0,  5,  SpectralClass::kWeibull  },
        {   10000,   2.1,   1.8,      25,   20.0,  5,  SpectralClass::kUniform  },
        {   7500,    1.4,   1.4,       5,   25.0,  4,  SpectralClass::kUniform  },
        {   6000,   1.04,  1.15,     1.5,   30.0,  3,  SpectralClass::kUniform  },
        {   5200,    0.8,  0.96,     0.6,   35.0,  3,  SpectralClass::kUniform  },
        {   3700,   0.45,   0.7,    0.08,   45.0,  2,  SpectralClass::kUniform  },
        {   2400,   0.08,   0.1,  5.3e-6,   50.0,  1,  SpectralClass::kUniform  }
    };
    /*
    vector<BuildCellFunction::SpectralInput> spectralInputs =
    {
        { 0.0 },
        { 0.00003 },
        { 0.13 },
        { 0.6 },
        { 3.0 },
        { 7.6 },
        { 12.1 },
        { 76.45 }
    };
    */
    constexpr double kSolarMassTotal = 20.0;
    vector<BuildStarmapFunction::SpectralInput> spectralInputs =
    {
        { 0.0 },
        { kSolarMassTotal * 0.0 },
        { kSolarMassTotal * 0.01 },
        { kSolarMassTotal * 0.09 },
        { kSolarMassTotal * 0.25 },
        { kSolarMassTotal * 0.35 },
        { kSolarMassTotal * 0.20 },
        { kSolarMassTotal * 0.10 }
    };
    ckm::vec3 cellMin( -8,-8,-8 );
    ckm::vec3 cellMax( 8, 8, 8 );
    ckm::AABB<ckm::vec3> cellBounds(cellMin, cellMax);
    
    auto cell = cellGenFn(10245,
        spectralClasses,
        spectralInputs,
        7,
        cellBounds,
        0.1, 1.5);
    
    if (cell.result == BuildStarmapFunction::Result::kSuccess)
    {
        //  merge results with our global entity store
        MergeStarmapWithEntityStore mergeFn;
        mergeFn(cell, _API.entityStore());

        _starmap = allocate_unique<Starmap>(_allocator);
        _starmap->tree = std::move(cell.stellarSystemTree);
    }
    
    RenderInterface _render(_API);
    _buildStarmapRenderHandler = _render.registerObjectListHandler(
        [this](const ckm::Frustrum& frustrum, RenderObjectListWriter& writer)
        {
            _starmap->buildObjectList(_API.entityStore(), frustrum, writer);
        }
    );

    //  create camera
    _camera = _API.entityStore().create();
    auto camera = _API.entityStore().table<overview::component::Camera>()
        .addComponentToEntity(_camera);
    if (camera)
    {
        camera->init(0, 90/M_PI, 0.1, 10000.0);
    }
    //  create entities from cell
    //  iterate through systems, creating entities from systems and bodies
    
    /*
    _bodyEntityId = _API.createEntity(
        kDocumentId_EntityTemplates,
        "star"
    );
    */

    //bx::mtxScale(mtx, 2.0f, 2.0f, 2.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    //bx::mtxTranslate(mtx, 0.0f, 0.0f, 0.0f);
    //bx::mtxMul(entityTransform->matrix, entityTransform->matrix, mtx);
    
    /*
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

void GameTask::onUpdate(uint32_t deltaTimeMs)
{
    //Matrix4 rotMat;
    //bx::mtxIdentity(rotMat);
    //bx::mtxRotateXYZ(rotMat, 0.f, bx::piHalf*0.10f*deltaTimeMs/1000.f, 0.f);
    
    /*
    auto object = _API.entityById(_bodyEntityId);
    if (object)
    {
        auto& entityMatrix = object->matrix();
        Matrix4 transform;
        
        bx::mtxMul(transform, entityMatrix, rotMat);
        object->matrix() = transform;
    }
    */
}

void GameTask::onEnd()
{
    _API.entityStore().destroy(_camera);
    
    RenderInterface(_API).unregisterObjectListHandler(_buildStarmapRenderHandler);
    _starmap = nullptr;
}

void GameTask::onFail()
{
}


} /* namespace ovproto */ } /* namespace cinek */
