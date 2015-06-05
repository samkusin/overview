//
//  MeshLibrary.cpp
//  Overview
//
//  Created by Samir Sinha on 5/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "MeshLibrary.hpp"

#include <cinek/debug.hpp>
#include <cinek/string.hpp>
#include <cinek/file.hpp>
#include <algorithm>

namespace cinek {
    namespace gfx {
    
bool MeshLibrary::Record::operator==(const char* texName) const
{
    return !strcmp(name, texName);
}

bool MeshLibrary::Record::matchesHandle(TextureHandle h) const
{
    return (h.data.iter == handleIter);
}


////////////////////////////////////////////////////////////////////////////////

MeshLibrary::MeshLibrary
(
    uint16_t meshCount,
    const Allocator& allocator
) :
    _allocator(allocator),
    _meshes(allocator)
{
    _meshes.reserve(meshCount);
}

MeshLibrary::MeshLibrary(MeshLibrary&& other) :
    _allocator(std::move(other._allocator)),
    _nextHandle(std::move(_nextHandle)),
    _meshes(std::move(_meshes))
{
}

MeshLibrary& MeshLibrary::operator=(MeshLibrary&& other)
{
    _allocator = std::move(other._allocator);
    _nextHandle = std::move(other._nextHandle);
    _meshes = std::move(other._meshes);
    return *this;
}

MeshLibrary::~MeshLibrary()
{
    for (auto& mesh : _meshes)
    {
        destroy(mesh);
    }
    _meshes.clear();
    _freed.clear();
}

void MeshLibrary::destroy(Record& record)
{
    if (record.name)
    {
        _allocator.free(record.name);
        record.name = nullptr;
    }
    record.mesh = nullptr;
    record.handleIter = 0;
}

MeshHandle MeshLibrary::load(const char* path)
{
    //  find texture with existing path - if it doesn't exist, create it
    MeshHandle handle = handleFromName(path);
    if (!handle)
    {
        CK_LOG_ERROR("MeshLibrary.load - unimplemented %s", path);
    }
    else
    {
        Record& record = _meshes[handle.data.offs];
        if (!record.matchesHandle(handle))
            return MeshHandle();
        ++_meshes[handle.data.offs].refCnt;
    }
    
    return handle;
        
}

MeshHandle MeshLibrary::acquire(unique_ptr<Mesh>&& mesh, const char* name)
{
    MeshHandle handle = handleFromName(name);
    if (!handle)
    {
        Record record;
        
        // allocate a texture handle
        handle = ++_nextHandle;
        if (_freed.empty())
        {
            handle.data.offs = (uint32_t)_meshes.size();
            _meshes.emplace_back();
        }
        else
        {
            handle.data.offs = _freed.back();
            _freed.pop_back();
        }
        
        record.mesh = std::move(mesh);
        record.name = duplicateCString(name, _allocator);
        record.handleIter = handle.data.iter;
        record.refCnt = 1;
    
        _meshes[handle.data.offs] = std::move(record);
    }
    else
    {
        Record& record = _meshes[handle.data.offs];
        if (!record.matchesHandle(handle))
            return MeshHandle();
        record.mesh = std::move(mesh);
        ++_meshes[handle.data.offs].refCnt;
    }
    
    return handle;
}

void MeshLibrary::unload(MeshHandle handle)
{
    if (!handle)
        return;
    
    Record& record = _meshes[handle.data.offs];
    if (!record.matchesHandle(handle))
        return;
    
    --record.refCnt;
    if (record.refCnt)
        return;
    
    destroy(record);
    _freed.push_back(handle.data.offs);
}

//  use to retrieve texture information.
MeshHandle MeshLibrary::handleFromName(const char* name) const
{
    auto it = std::find(_meshes.begin(), _meshes.end(), name);
    if (it == _meshes.end())
        return nullptr;
    
    MeshHandle h;
    h.data.iter = it->handleIter;
    h.data.offs = (uint32_t)(it - _meshes.begin());
    return h;
}

const Mesh* MeshLibrary::mesh(MeshHandle handle) const
{
    if (!handle)
        return nullptr;
    
    auto& mesh = _meshes[handle.data.offs];
    return mesh.mesh.get();
}
        
    }   // namespace gfx
}   // namespace cinek
