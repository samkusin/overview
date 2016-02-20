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
    ckm::vector3f endPos
) :
    _query(std::move(queryPtr)),
    _entity(entity),
    _startPos(startPos),
    _endPos(endPos)
{
    if (_query) {
        _path.reserve(_query->nodeLimit());
    }
}
    
GenerateNavPath::~GenerateNavPath()
{
    _query = nullptr;
}

std::vector<float> GenerateNavPath::acquirePoints()
{
    return std::move(_path);
}
    
    
    } /* namespace ove */
} /* namespace cinek */