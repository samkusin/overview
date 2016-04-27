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
    NavPath(std::vector<dtPolyRef>&& polys, ckm::vector3 startPos,
            ckm::vector3 targetPos);
    
    int regionCount() const { return (int)_polys.size(); }
    
    explicit operator bool() const { return !_polys.empty(); }
    
    ckm::vector3 startPos() const { return _startPos; }
    ckm::vector3 targetPos() const { return _targetPos; }
    
    int plot(float* points, unsigned char* flags, dtPolyRef* polys, int resultCount,
             NavPathQuery& query, const ckm::vector3& pos, ckm::scalar dist) const;
    
    const dtPolyRef* polys() const;
    int numPolys() const;
    
    bool updatePath(dtPolyRef poly);
    void clear();
    
private:
    //  contains poly references
    std::vector<dtPolyRef> _polys;
    //  current position on path
    ckm::vector3 _startPos;
    //  end position 
    ckm::vector3 _targetPos;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_NavPath_hpp */
