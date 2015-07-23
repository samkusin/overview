//
//  GalaxyViewController.cpp
//  Overview
//
//  Created by Samir Sinha on 7/18/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "GalaxyViewController.hpp"

#include "RenderInterface.hpp"
#include "CKGfx/ShaderLibrary.hpp"
#include "CKGfx/TextureAtlas.hpp"

#include "Engine/EngineMathGfx.hpp"

#include "Engine/Entity/TransformEntity.hpp"
#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"
#include "Custom/Comp/StellarSystem.hpp"
#include "Custom/Comp/StarBody.hpp"

#include "UI/UIBuilder.hpp"

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


struct GalaxyViewController::Starmap
{
    StellarSystemTree tree;
    
    void buildObjectList
    (
        const overview::EntityStore& store,
        const ckm::Frustrum& frustrum,
        RenderObjectList& writer
    )
    {
        struct Callback
        {
            const overview::component::EntityDataTable* renderables;
            RenderObjectList* writer;
            
            Callback(const overview::EntityStore& store,
                     RenderObjectList& writer) :
                renderables(store.table<overview::component::Renderable>().data()),
                writer(&writer)
            {
            }
            
            bool operator()(Entity entity) const
            {
                RenderObject obj;
                obj.renderableIdx = renderables->rowIndexFromEntity(entity);
                writer->push_back(obj);
                return true;
            }
        };
        
        Callback cb(store, writer);
        tree.test<StellarSystemTree::Test::FrustrumSweep>()(frustrum, cb);
    }
};


GalaxyViewController::GalaxyViewController(AppInterface api) :
    _API(api)
{
    _uniforms.fill(BGFX_INVALID_HANDLE);
    _renderables.reserve(16384);
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
    constexpr ckm::scalar kCellBoundZ = 32;
    constexpr ckm::scalar kCellBoundXY = 32;
    constexpr ckm::scalar kSolarMassTotal = 0.0008 * (kCellBoundXY*kCellBoundXY*kCellBoundZ*8);
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
        mergeFn(cell, _API.entityStore());

        _starmap = allocate_unique<Starmap>(_allocator);
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
    
    RenderInterface renderAPI(_API);
    
    _shaderProgram = renderAPI.renderResources().shaderLibrary->loadProgram(
        gfx::makeShaderProgramId(gfx::VertexTypes::kVec3_RGBA_Tex0, kShaderIndex_Star),
        "shaders/vs_starmap.bin",
        "shaders/fs_starmap.bin");
    
    _starCorona = renderAPI.renderResources().textureAtlas->loadTexture("textures/starcorona.png");
    
    bx::mtxIdentity(_mapTransform);

    //  create camera
    _camera = _API.entityStore().create();
    auto camera = _API.entityStore().table<overview::component::Camera>()
        .addComponentToEntity(_camera);
    if (camera)
    {
        camera->init(0, M_PI * 90/180.0 , 0.5, 500.0);
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

void GalaxyViewController::onViewUnload()
{
    _API.entityStore().destroy(_camera);
    
    _starmap = nullptr;
    
    overview::RenderResources& resources = RenderInterface(_API).renderResources();
    
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
        resources.textureAtlas->unloadTexture(_starCorona);
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

void GalaxyViewController::updateView()
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

void GalaxyViewController::layoutView()
{
    UILayout layout;
    
    auto viewRect = RenderInterface(_API).viewRect();
    
    layout.frame().
        events(UI_BUTTON0_DOWN, this, kID_VIEW).
        size(viewRect.w, viewRect.h).
        column(UI_FILL).
            button(UITHEME_ICON_GHOST, "Button", this, kID_BUTTON).
        end().
    end();
}

void GalaxyViewController::onUIEvent(int evtId, int evtType)
{
    if (evtId == kID_VIEW)
    {
        printf("View Hit\n");
    }
    else if (evtId == kID_BUTTON)
    {
        printf("Button Hit\n");
    }
}

struct RendererCameraTranslate
{
    component::Table<overview::component::Renderable> renderables;
    component::Table<overview::component::Transform> transforms;
    ckm::vec4 offset;
    
    void onEntity(Entity e, overview::component::Renderable& r)
    {
        auto transform = transforms.rowForEntity(e);
        if (transform)
        {
            auto srt = transform->worldSRT();
            srt[3] -= offset;
            ckm::convert(srt, r.worldSRT);
            
            //  iterate through children
            Entity child = transform->child();
            while (child.valid())
            {
                auto childRenderable = renderables.rowForEntity(child);
                
                if (childRenderable)
                {
                    onEntity(child, *childRenderable);
                }
                auto childTransform = transforms.rowForEntity(child);
                child = childTransform->sibling();
            }
        }
    }
};

void GalaxyViewController::renderView()
{
    if (!_shaderProgram)
        return;
    
    _renderables.clear();
    
    auto viewRect = RenderInterface(_API).viewRect();
    
    bgfx::setViewRect(0, viewRect.x, viewRect.y, viewRect.w, viewRect.h);
    
    bgfx::submit(0);
            
    //  calculate camera frustrum (view-space) and the view-to-world
    //  transformation used by object list providers to cull entities
    //  from world space

    auto& entityStore = _API.entityStore();
    auto transformTable = entityStore.table<overview::component::Transform>();
    auto cameraTransform = transformTable.rowForEntity(_camera);
    auto camera = entityStore.table<overview::component::Camera>().rowForEntity(_camera);
            
    ckm::mat4 cameraSRT = cameraTransform ? cameraTransform->worldSRT() : ckm::mat4(1);
    ckm::mat3 cameraBasis(cameraSRT);
        
    auto aspect = (ckm::scalar)viewRect.w / viewRect.h;
            
    ckm::Frustrum worldFrustrum;
            
    worldFrustrum = ckm::Frustrum(camera->nearZ(),
                    camera->farZ(),
                    camera->fov(),
                    aspect
                ).transform(
                    cameraBasis,
                    ckm::vec3(cameraSRT[3])
                );
    
    _starmap->buildObjectList(_API.entityStore(), worldFrustrum, _renderables);
    
    
    //  We translate renderable objects so that our render camera is at
    //  the origin.  Since its possible world objects use high precision
    //  coordinates, to avoid precision errors when dealing with render
    //  pipeline coordinates (always float) we translate coordinates
    //  into floats.  Of course this assumes we're not rendering objects
    //  at a distance beyond the float precision range (why would one
    //  do that?!)
    //
    //  Rotations and scale remain as they are in world space
    auto renderables = entityStore.table<overview::component::Renderable>();

    RendererCameraTranslate cameraTranslate;
    cameraTranslate.offset = cameraSRT[3];
    cameraTranslate.renderables = renderables;
    cameraTranslate.transforms = transformTable;
    
    for (auto& renderObject : _renderables)
    {
        auto renderable = renderables.rowAtIndex(renderObject.renderableIdx);
        if (renderable.second)
        {
            cameraTranslate.onEntity(renderable.first, *renderable.second);
        }
    }
    
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
    
    struct SystemVisitor : public overview::component::TransformVisitor
    {
        component::Table<overview::component::Renderable> renderables;
        component::Table<component::StarBody> stars;
        Vertex* vertices;
        int starIndex;
        uint16_t* indices;
        
        bool visit(overview::Entity e, overview::component::Transform& t)
        {
            auto star = stars.rowForEntity(e);
            if (star)
            {
                int vertIndex = starIndex*4;
                auto renderable = renderables.rowForEntity(e);
                
                auto vert = vertices + vertIndex;
                
                //  set position
                vert->x = renderable->worldSRT.comp[12];
                vert->y = renderable->worldSRT.comp[13];
                vert->z = renderable->worldSRT.comp[14];
                
                //  color
                vert->abgr = star->abgrColor;
                
                //  set attributes
                vert->u = star->visualMag;
                
                auto base = vert;
                
                for (vert = vert+3; vert > base; --vert)
                {
                    *vert = *base;
                }
                
                /*
                
                vert[0].x = -2.0f;
                vert[0].y = 2.0f;
                vert[0].z = 5.0f;
                vert[0].abgr = star->abgrColor;
                vert[1].x = 2.0f;
                vert[1].y = 2.0f;
                vert[1].z = 5.0f;
                vert[1].abgr = star->abgrColor;
                vert[2].x = 2.0f;
                vert[2].y = -2.0f;
                vert[2].z = 5.0f;
                vert[2].abgr = star->abgrColor;
                vert[3].x = -2.0f;
                vert[3].y = -2.0f;
                vert[3].z = 5.0f;
                vert[3].abgr = star->abgrColor;
                */
                
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
    
    auto& shaderLibrary = RenderInterface(_API).renderResources().shaderLibrary;
    auto& textureAtlas = RenderInterface(_API).renderResources().textureAtlas;
    
    bgfx::setUniform(_uniforms[kUniform_Scale], &scale);
    bgfx::TextureHandle texture0 = textureAtlas->texture(_starCorona);
    bgfx::setTexture(0, _uniforms[kUniform_Texture0], texture0);
    
    bgfx::ProgramHandle bgfxProgram = shaderLibrary->program(_shaderProgram);
    bgfx::setProgram(bgfxProgram);
    
    auto systems = entityStore.table<component::StellarSystem>();
    auto transforms = entityStore.table<overview::component::Transform>();
    
    SystemVisitor systemVisitor;
    systemVisitor.renderables = renderables;
    systemVisitor.stars = entityStore.table<component::StarBody>();
    
    //  generate star vertices, color table indices and absolute magnitudes
    while (!_renderables.empty())
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

        for (; !_renderables.empty() &&
               (systemVisitor.starIndex + kMaxStarsPerSystem) < kStarLimit; )
        {
            auto object = _renderables.back();
            _renderables.pop_back();
            
            auto renderable = renderables.rowAtIndex(object.renderableIdx);
            auto entity = renderable.first;
            
            auto system = systems.rowForEntity(entity);
            if (system)
            {
                systemVisitor(entity, transforms);
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
                | BGFX_STATE_CULL_CCW
                | BGFX_STATE_MSAA
                | BGFX_STATE_BLEND_ALPHA
            );
                    
            bgfx::submit(0);
        }
    }
}

} /* namespace ovproto */

} /* namespace cinek */

