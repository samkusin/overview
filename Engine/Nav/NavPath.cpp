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

NavPath::NavPath(std::vector<dtPolyRef>&& polys) :
    _polys(std::move(polys))
{
}
    
    } /* namespace ove */
} /* namespace cinek */
