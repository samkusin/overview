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
    
    _placeholderMesh = createCube(1.0, VertexTypes::kVec3_Normal_RGBA, {{0,1,0}});
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
    for (int i = 0; i < LODIndex::kCount; ++i)
    {
        record.levels[i] = nullptr;
        record.levelIndex[i] = -1;
    }
    record.handleIter = 0;
}

MeshHandle MeshLibrary::load(const char* path)
{
    //  find texture with existing path - if it doesn't exist, create it
    MeshHandle handle = handleFromName(path);
    if (!handle)
    {
        CK_LOG_ERROR("ckgfx", "MeshLibrary.load - unimplemented %s", path);
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

MeshHandle MeshLibrary::create(const char* name)
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
        
        //  create reference to placeholder
        for (int i = 0; i < LODIndex::kCount; ++i)
        {
            record.levelIndex[i] = LODIndex::kSmall;
            record.levels[i] = nullptr;
        }
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
        ++_meshes[handle.data.offs].refCnt;
    }
    return handle;
}

void MeshLibrary::setMeshForLOD
(
    MeshHandle handle,
    unique_ptr<Mesh>&& mesh,
    LODIndex::Enum lod
)
{
    if (!handle)
        return;

    Record& record = _meshes[handle.data.offs];
    if (!record.matchesHandle(handle))
        return;
    
    record.levels[lod] = std::move(mesh);
    
    //  remap lod level indices, preferring higher LODs
    int meshIndex = 0;
    int lodToMeshIndex = 0;
    for (; meshIndex < LODIndex::kCount; ++meshIndex)
    {
        if (record.levels[meshIndex])
        {
            while (lodToMeshIndex < LODIndex::kCount &&
                   (!record.levels[lodToMeshIndex] || lodToMeshIndex != meshIndex))
            {
                record.levelIndex[lodToMeshIndex] = meshIndex;
                ++lodToMeshIndex;
            }
        }
        ++meshIndex;
    }
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

const Mesh& MeshLibrary::mesh(MeshHandle handle, LODIndex::Enum lod) const
{
    if (handle)
    {
        auto& record = _meshes[handle.data.offs];
        auto index = record.levelIndex[lod];
        if (index >= 0)
        {
            auto& level = record.levels[index];
            if (level)
            {
                return *level.get();
            }
        }
    }
    return *_placeholderMesh.get();
}
        
    }   // namespace gfx
}   // namespace cinek
