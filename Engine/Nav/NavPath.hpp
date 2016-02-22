//
//  NavPath.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#ifndef Overview_NavPath_hpp
#define Overview_NavPath_hpp

#include "PathTypes.hpp"
#include "Engine/Contrib/Recast/DetourNavMesh.h"

#include <vector>

namespace cinek {
    namespace ove {

class NavPath
{
public:
    NavPath();
    NavPath(std::vector<dtPolyRef>&& polys);
    
    int size() const { return (int)_polys.size(); }
    
private:
    //  contains poly references
    std::vector<dtPolyRef> _polys;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_NavPath_hpp */
