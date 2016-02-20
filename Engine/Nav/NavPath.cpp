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

NavPath::NavPath(std::vector<float>&& points) :
    _points(std::move(points))
{
}
    
    } /* namespace ove */
} /* namespace cinek */
