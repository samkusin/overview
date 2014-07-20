//
//  Stage.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved.
//  Author: Samir Sinha
//  License: The MIT License (MIT)
//

#include "Engine/Model/Stage.hpp"

#include "Engine/Debug.hpp"
#include "Engine/Model/TileLibrary.hpp"
#include "Engine/Model/SpriteLibrary.hpp"

namespace cinekine { namespace ovengine {

inline TileId CompressTile(TileSlot slot, TileIndex index)
{
    return (slot << 12) + (index & 0xf000);
}

inline TileSlot SlotFromTileId(TileId id)
{
    return (id >> 12);
}

inline TileIndex IndexFromTileId(TileId id)
{
    return (id & 0xf000);
}


Stage::Stage(const TileLibrary& tileDb,
             const SpriteLibrary& spriteDb,
             const ResourceCounts& counts,
             const MapBounds& bounds,
             const Allocator& allocator) :
    _tileDb(tileDb),
    _spriteDb(spriteDb),
    _bounds(bounds),
    _tilemaps(std_allocator<TileGrid>(allocator)),
    _spriteInstances(std_allocator<StageSpriteInstance>(allocator)),
    _freeSpriteInstanceIds(std_allocator<uint32_t>(allocator))
{
    _spriteInstances.reserve(counts.spriteLimit);
    _freeSpriteInstanceIds.reserve(counts.spriteLimit);

    //  validation
    //
    if (!_bounds.xUnits || !_bounds.yUnits || !_bounds.zUnits)
    {
        OVENGINE_LOG_ERROR("Stage - Invalid bounds specified (%u x %u)", _bounds.xUnits, _bounds.yUnits);
        _bounds.xUnits = 1;
        _bounds.yUnits = 1;
        _bounds.zUnits = 1;
    }

    //  allocate tilemaps
    //      Tilemaps are not copyable - so can't initialize via initializer lists.
    _tilemaps.reserve(_bounds.zUnits);
    for (int32_t i = 0; i < _bounds.zUnits; ++i)
    {
        _tilemaps.push_back({ _bounds.xUnits, _bounds.yUnits, allocator});
    }
}

TileGrid* Stage::tileGridAtZ(int16_t z)
{
    if (z < 0 || z >= _bounds.zUnits)
        return nullptr;
    return &_tilemaps[z];
}

const TileGrid* Stage::tileGridAtZ(int16_t z) const
{
    if (z < 0 || z >= _bounds.zUnits)
        return nullptr;
    return &_tilemaps[z];
}

const Tile& Stage::tile(TileId tileId) const
{
    return _tileDb.tileFromCollectionAtIndex(SlotFromTileId(tileId),
                                             IndexFromTileId(tileId));
}

std::pair<SpriteInstanceId, StageSpriteInstance*>
        Stage::createSpriteInstance(const string& name,
                                    const glm::vec3& pos)
{
    StageSpriteInstance* instance = nullptr;
    SpriteInstanceId id = 0;
    auto spriteTemplate = _spriteDb.spriteByName(name);
    if (spriteTemplate)
    {
        if (!_freeSpriteInstanceIds.empty())
        {
            //  take instance off the free list first.  destroy and recreate
            //  it
            id = _freeSpriteInstanceIds.back();
            _freeSpriteInstanceIds.pop_back();
            instance = &_spriteInstances[id-1];
            instance->~StageSpriteInstance();
            ::new(instance) StageSpriteInstance(*spriteTemplate, *this, id);
        }
        else if (_spriteInstances.size() != _spriteInstances.capacity())
        {
            //  create new instance on the heap
            id = _spriteInstances.size() + 1;
            _spriteInstances.emplace_back(*spriteTemplate, *this, id);
            instance = &_spriteInstances.back();
        }
        if (instance)
        {
            instance->setPosition(pos);
        }
    }

    return { id, instance };
}

StageSpriteInstance* Stage::spriteInstance(SpriteInstanceId instanceHandle)
{
    if (!instanceHandle || instanceHandle > _spriteInstances.size())
        return nullptr;
    if (!_spriteInstances[instanceHandle-1])
        return nullptr;
    return &_spriteInstances[instanceHandle-1];
}

void Stage::releaseSpriteInstance(SpriteInstanceId instanceHandle)
{
    if (instanceHandle > _spriteInstances.size())
        return;
    auto& instance = _spriteInstances[instanceHandle-1];
    //  avoid killing instances that are already dead
    if (!instance)
        return;
    //  kill instance and push back its handle onto the free list for
    //  reuse
    instance.reset();
    _freeSpriteInstanceIds.push_back(instanceHandle);
}

} /* namespace ovengine */ } /* namespace cinekine */

