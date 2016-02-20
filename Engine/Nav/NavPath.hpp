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

#include <vector>

namespace cinek {
    namespace ove {

class NavPath
{
public:
    NavPath();
    NavPath(std::vector<float>&& points);
    
private:
    //  contains x,y,z tuples
    std::vector<float> _points;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_NavPath_hpp */
