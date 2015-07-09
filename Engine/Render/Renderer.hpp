//
//  RenderScene.hpp
//  Overview
//
//  Created by Samir Sinha on 7/5/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Render_RenderScene_hpp
#define Overview_Render_RenderScene_hpp

#include "RenderTypes.hpp"
#include "Engine/AABB.hpp"

#include <glm/vec2.hpp>

#include <cinek/vector.hpp>
#include <cinek/map.hpp>
#include <functional>

namespace cinek { namespace overview {

struct RenderView
{
    ckm::AABB2<ckm::ivec2> rect;
    uint32_t clearColor;
    float clearDepth;
    uint8_t clearStencil;
    bool hasDepth : 1;
    bool hasStencil : 1;
    
    operator bool() const {
        return rect;
    }
};

struct RenderCommand
{
    struct Params
    {
        int32_t pipelineIdx;
    };
    Params params;
    int32_t listStartIndex;
    int32_t listEndIndex;
};

using RenderObjectList = vector<RenderObject>;
using RenderCommandList = vector<RenderCommand>;

class RenderObjectListWriter
{
public:
    RenderObjectListWriter(RenderCommandList& cmdList, RenderObjectList& objList);
    
    void beginCommand(const RenderCommand::Params& params);
    void endCommand();
    void insertObject(const RenderObject& obj);
    
private:
    RenderCommandList* _commandList;
    RenderObjectList* _renderList;
    int32_t _objectIndex;
};

class RenderObjectListReader
{
public:
    RenderObjectListReader(const RenderCommand& cmd, RenderObjectList& objList);
    
    bool empty() const { return _command.listStartIndex == _command.listEndIndex; }
    int32_t size() const { return _command.listEndIndex - _command.listStartIndex; }
    
    RenderObject pop();

private:
    RenderCommand _command;
    RenderObjectList* _renderList;
};


class Renderer
{
public:
    struct InitParams
    {
        int32_t pipelineCnt;
        int32_t objectCnt;
        int32_t commandCnt;
        int32_t width;
        int32_t height;
    };
    Renderer(const InitParams& params, const Allocator& allocator=Allocator());
    
    //  Note - View 0 is created by default to encompass the whole screen
    uint32_t viewLimit() const { return (uint32_t)_views.size(); }
    void initView(uint32_t index, const RenderView& view);
    
    //  Note - do not call these methods outside of the main thread
    Handle onBuildObjectList(const BuildRenderObjectListCb& cb);
    void delBuildObjectListHandler(Handle handle);
    
    //  these methods extend the functionality of the Renderer
    void setPipelineCallback(int32_t pipeline, const RenderPipelineCb& cb);
    void clearPipelineCallback(int32_t pipeline);

    void render(RenderContext context);
    
private:
    unordered_map<Handle, BuildRenderObjectListCb> _buildObjectListHandlers;
    vector<RenderPipelineCb> _pipelines;
    RenderCommandList _commands;
    RenderObjectList _objects;
    Handle _listHandlerHandle;
    
    vector<RenderView> _views;
};


} /* namespace overview */ } /* namespace cinek */

#endif /* defined(__Overview__RenderScene__) */
