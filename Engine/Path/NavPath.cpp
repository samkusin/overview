//
//  NavPath.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#include "NavPath.hpp"

#include <algorithm>

namespace cinek {
    namespace ove {
    
NavPath::NavPath()
{
}

NavPath::NavPath
(
    std::vector<dtPolyRef>&& polys,
    ckm::vector3 startPos,
    ckm::vector3 targetPos
) :
    _polys(std::move(polys)),
    _startPos(startPos),
    _targetPos(targetPos)
{
}

const dtPolyRef* NavPath::polys() const
{
    return _polys.data();
}

int NavPath::numPolys() const
{
    return (int)_polys.size();
}


bool NavPath::updatePath(dtPolyRef poly)
{
    auto it = std::find(_polys.begin(), _polys.end(), poly);
    if (it == _polys.end()) {
        return false;
    }
    
    _polys.erase(_polys.begin(), it);
    return true;
}

void NavPath::clear()
{
    _polys.clear();
}
    
    } /* namespace ove */
} /* namespace cinek */
