//
//  RenderScene.cpp
//  Overview
//
//  Created by Samir Sinha on 7/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "Renderer.hpp"

#include "Engine/Entity/EntityStore.hpp"
#include "Engine/Entity/Comp/Camera.hpp"
#include "Engine/Entity/Comp/Transform.hpp"
#include "Engine/Entity/Comp/Renderable.hpp"

#include "Engine/EngineGeometry.hpp"
#include "Engine/EngineMath.hpp"
#include "Engine/EngineMathGfx.hpp"

#include <glm/ext.hpp>

namespace cinek { namespace overview {

RenderObjectListWriter::RenderObjectListWriter
(
    RenderCommandList& cmdList,
    RenderObjectList& objList
) :
    _commandList(&cmdList),
    _renderList(&objList),
    _objectIndex(-1)
{
}
 
void RenderObjectListWriter::beginCommand
(
    const RenderCommand::Params& params
)
{
    endCommand();

    _commandList->emplace_back();
    
    auto& cmd = _commandList->back();
    cmd.params = params;
    cmd.listStartIndex = (int32_t)_renderList->size();
    cmd.listEndIndex = -1;
    _objectIndex = cmd.listStartIndex;
}

void RenderObjectListWriter::endCommand()
{
    if (_commandList->empty() || _objectIndex < 0)
        return;
    
    auto& cmd = _commandList->back();
    if (cmd.listStartIndex < 0 || cmd.listEndIndex >= 0)
        return;
    
    cmd.listEndIndex = (int32_t)_renderList->size();
}


void RenderObjectListWriter::insertObject
(
    const RenderObject& obj
)
{
    if (_commandList->empty() || _objectIndex < 0)
        return;
    _renderList->push_back(obj);
}

RenderObjectListReader::RenderObjectListReader
(
    const RenderCommand& cmd,
    RenderObjectList& objList
) :
    _command(cmd),
    _renderList(&objList)
{
}
    
RenderObject RenderObjectListReader::pop()
{
    if (_command.listStartIndex >= _command.listEndIndex ||
        _command.listEndIndex > _renderList->size())
        return RenderObject();
    
    return _renderList->at(_command.listStartIndex++);
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

////////////////////////////////////////////////////////////////////////////////

Renderer::Renderer
(
    const InitParams& params,
    const Allocator& allocator
) :
    _buildObjectListHandlers(allocator),
    _pipelines(allocator),
    _commands(allocator),
    _objects(allocator),
    _listHandlerHandle(kNullHandle)
{
    _pipelines.resize(params.pipelineCnt);
    _commands.reserve(params.commandCnt);
    _objects.reserve(params.objectCnt);

    const bgfx::Caps* halCaps = bgfx::getCaps();
    _views.resize(halCaps->maxViews);
    
    for (auto& view : _views)
    {
        view.rect = ckm::ivec2(0,0);
    }
    
    RenderView view0;
    view0.rect.min = ckm::ivec2(0);
    view0.rect.max = ckm::ivec2(params.width, params.height);
    view0.clearColor = 0x000011ff,
    view0.clearDepth = 1.0f;
    view0.clearStencil = 0;
    view0.hasDepth = true;
    view0.hasStencil = false;
    
    initView(0, view0);
}

void Renderer::initView(uint32_t index, const RenderView& view)
{
    if (index >= _views.size())
        return;
    
    _views[index] = view;
    auto& renderView = _views[index];
    
    uint32_t flags = BGFX_CLEAR_COLOR;
    if (renderView.hasDepth)
    {
        flags |= BGFX_CLEAR_DEPTH;
    }
    else
    {
        renderView.clearDepth = 0.f;
    }
    if (renderView.hasStencil)
    {
        flags |= BGFX_CLEAR_STENCIL;
    }
    else
    {
        renderView.clearStencil = 0;
    }
    
    bgfx::setViewClear(index, flags,
        renderView.clearColor,
        renderView.clearDepth,
        renderView.clearStencil);
}
    
Handle Renderer::onBuildObjectList(const BuildRenderObjectListCb& cb)
{
    ++_listHandlerHandle;
    if (_listHandlerHandle == kNullHandle)
        _listHandlerHandle = 1;
    
    _buildObjectListHandlers.emplace(_listHandlerHandle, cb);
    
    return _listHandlerHandle;
}

void Renderer::delBuildObjectListHandler(Handle handle)
{
    _buildObjectListHandlers.erase(_listHandlerHandle);
}

void Renderer::setPipelineCallback
(
    int32_t pipeline,
    const RenderPipelineCb& cb
)
{
    if (pipeline >= _pipelines.size())
    {
        _pipelines.resize(pipeline);
    }
    _pipelines[pipeline] = cb;
}

void Renderer::clearPipelineCallback(int32_t pipeline)
{
    if (pipeline >= _pipelines.size())
        return;
    
    _pipelines[pipeline] = RenderPipelineCb();
}

void Renderer::render(RenderContext context)
{
    //  one view = one camera
    //  every view populates a render list
    
    auto cameraTable = context.entityStore->table<overview::component::Camera>();
    if (!cameraTable)
        return;
    
    struct LocalContext
    {
        RenderContext* rc;
        Renderer* self;
    };
    LocalContext lc = { &context, this };
    
    cameraTable.forEach(
        [&lc](Entity e, const overview::component::Camera& camera)
        {
            auto& entityStore = *lc.rc->entityStore;
            
            int viewId = camera.viewIndex();
            auto& view = lc.self->_views[viewId];
            auto& commands = lc.self->_commands;
            auto& objects = lc.self->_objects;
            auto& objectListHandlers = lc.self->_buildObjectListHandlers;
            auto& pipelines = lc.self->_pipelines;
            
            auto viewDim = view.rect.dimensions();
            bgfx::setViewRect(viewId, view.rect.min.x, view.rect.min.y,
                viewDim.x, viewDim.y);
            
            bgfx::submit(viewId);
            
            //  calculate camera frustrum (view-space) and the view-to-world
            //  transformation used by object list providers to cull entities
            //  from world space
            auto transformTable = entityStore.table<overview::component::Transform>();
            auto cameraTransform = transformTable.rowForEntity(e);
            
            ckm::mat4 cameraSRT = cameraTransform ? cameraTransform->worldSRT() : ckm::mat4(1);
            ckm::mat3 cameraBasis(cameraSRT);
        
            auto aspect = (ckm::scalar)viewDim.x / viewDim.y;
            
            auto worldFrustrum = ckm::Frustrum(camera.nearZ(),
                                    camera.farZ(),
                                    camera.fov(),
                                    aspect
                                 ).transform(
                                    cameraBasis,
                                    ckm::vec3(cameraSRT[3])
                                 );
            
            //  construct our render list
            RenderObjectListWriter listWriter(commands, objects);
            
            for (auto& listHandler : objectListHandlers)
            {
                listHandler.second(worldFrustrum, listWriter);
            }
            
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
            
            for (auto& renderObject : objects)
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
            
            //  generate our bgfx view and projection matrices
            //  2d notes: if the camera is 2d, we create an orthogonal matrix
            //  general notes: near and far are assumed to be 'reasonable'
            //  values (meaning within float range)
            //
            RenderCameraContext cameraContext(viewId);
            bx::mtxInverse(cameraContext.mtxView,
                ckm::convert(cameraSRT, cameraContext.mtxView));
            bx::mtxProj(cameraContext.mtxProj,
                180.0f * (camera.fov()/M_PI), (float)aspect,
                (float)camera.nearZ(),
                (float)camera.farZ());
            
            //  run commands through the various registered pipelines
            for (auto& command : commands)
            {
                int32_t pipelineIdx = command.params.pipelineIdx;
                if (pipelineIdx >= 0 && pipelineIdx < pipelines.size())
                {
                    auto& pipeline = pipelines[command.params.pipelineIdx];
                    if (pipeline)
                    {
                        RenderObjectListReader listReader(command, objects);
                        pipeline(*lc.rc, cameraContext, listReader);
                    }
                }
            }
            
            commands.clear();
            objects.clear();
        }
    );
}

} /* namespace overview */ } /* namespace cinek */