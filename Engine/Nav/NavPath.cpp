//
//  NavPath.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#include "NavPath.hpp"

namespace cinek {
    namespace ove {
    
NavPath::NavPath()
{
}

NavPath::NavPath
(
    std::vector<dtPolyRef>&& polys,
    ckm::vector3f startPos,
    ckm::vector3f targetPos
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
    
    } /* namespace ove */
} /* namespace cinek */
