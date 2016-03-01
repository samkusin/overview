//
//  NavPathQueryPool.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#ifndef Overview_NavPathQueryPool_hpp
#define Overview_NavPathQueryPool_hpp

#include "PathTypes.hpp"

#include <cinek/objectpool.hpp>

#include <vector>

namespace cinek {
    namespace ove {
    
class NavPathQueryPool
{
public:
    struct InitParams
    {
        const NavMesh* navMesh;
        uint32_t numQueries;
    };
    
    NavPathQueryPool(const InitParams& initParams);
    
    NavPathQueryPtr acquire();
    
private:
    friend struct NavPathQueryDeleter;
    
    void release(NavPathQuery* query);
    
    const NavMesh* _mesh;
    ObjectPool<NavPathQuery> _defaultQueryPool;
    std::vector<NavPathQuery*> _defaultQueries;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* NavPathQueryPool_hpp */
