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
    NavPath(std::vector<dtPolyRef>&& polys, ckm::vector3f startPos,
            ckm::vector3f targetPos);
    
    int regionCount() const { return (int)_polys.size(); }
    
    explicit operator bool() const { return !_polys.empty(); }
    
    ckm::vector3f startPos() const { return _startPos; }
    ckm::vector3f targetPos() const { return _targetPos; }
    
    int plot(float* points, unsigned char* flags, dtPolyRef* polys, int resultCount,
             NavPathQuery& query, const ckm::vector3f& pos, ckm::scalar dist) const;
    
    const dtPolyRef* polys() const;
    int numPolys() const;
    
    bool updatePath(dtPolyRef poly);
    void clear();
    
private:
    //  contains poly references
    std::vector<dtPolyRef> _polys;
    //  current position on path
    ckm::vector3f _startPos;
    //  end position 
    ckm::vector3f _targetPos;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_NavPath_hpp */
