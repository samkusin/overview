//
//  GenerateNavPath.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#include "GenerateNavPath.hpp"

#include "Engine/Nav/NavPathQueryPool.hpp"
#include "Engine/Nav/NavPathQuery.hpp"

namespace cinek {
    namespace ove {
    
GenerateNavPath::GenerateNavPath
(
    NavPathQueryPtr&& queryPtr,
    Entity entity,
    ckm::vector3f startPos,
    ckm::vector3f endPos,
    ckm::vector3f extents,
    const Allocator& allocator
) :
    _allocator(allocator),
    _query(std::move(queryPtr)),
    _entity(entity),
    _startPos(startPos),
    _endPos(endPos),
    _extents(extents),
    _polyPath(nullptr),
    _polyPathIndex(0),
    _polyPathSize(0)
{
    if (_query) {
        int nodeCount = _query->nodeLimit();
        _polyPath = _allocator.allocItems<dtPolyRef>(nodeCount);
    }
}
    
GenerateNavPath::~GenerateNavPath()
{
    if (_polyPath) {
        _allocator.free(_polyPath);
        _polyPath = nullptr;
    }
    _query = nullptr;
}

std::vector<dtPolyRef> GenerateNavPath::acquirePoints()
{
    std::vector<dtPolyRef> path(_polyPath, _polyPath+_polyPathSize);
    return std::move(path);
}

void GenerateNavPath::onBegin()
{
    //  generates the polygon path
    dtPolyRef startRef;
    dtPolyRef endRef;
    
    auto& queryInterface = _query->interface();
    auto& queryFilter = _query->filter();
    
    queryInterface.findNearestPoly(_startPos.comp, _extents.comp, &queryFilter,
                            &startRef, nullptr);
    
    queryInterface.findNearestPoly(_endPos.comp, _extents.comp, &queryFilter,
                            &endRef, nullptr);
    
    queryInterface.findPath(startRef, endRef, _startPos.comp, _endPos.comp, &queryFilter,
                            _polyPath, &_polyPathSize, _query->nodeLimit());

    if (_polyPathSize <= 0) {
        end();
        return;
    }
}

void GenerateNavPath::onUpdate(uint32_t /* deltaTimeMs */)
{
    if (_polyPathIndex >= _polyPathSize) {
        end();
        return;
    }
    
    //  generate the 'smooth' path from start to end
    //
    //  - the 'start' advances each iteration of this task
    //  - we advance a few points every iteration
    //  - we fill the path vector with these generated points
    //
    const int kPolysPerIteration = _polyPathSize;
    _polyPathIndex += kPolysPerIteration;
}
    
    
    } /* namespace ove */
} /* namespace cinek */