//
//  NavPathQueryPool.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#include "NavPathQueryPool.hpp"
#include "NavPathQuery.hpp"

#include <cinek/objectpool.inl>

namespace cinek {

template class ObjectPool<ove::NavPathQuery>;


namespace ove {
    
NavPathQueryPool::NavPathQueryPool(const InitParams& initParams) :
    _mesh(initParams.navMesh),
    _defaultQueryPool(initParams.numQueries)
{
    _defaultQueries.reserve(initParams.numQueries);
    
    for (uint32_t i = 0; i < initParams.numQueries; ++i) {
        NavPathQuery* query = _defaultQueryPool.construct(_mesh, 1024);
        _defaultQueries.emplace_back(query);
    }
}

NavPathQueryPtr NavPathQueryPool::acquire()
{
    CK_ASSERT_RETURN_VALUE(!_defaultQueries.empty(), nullptr);
    
    NavPathQuery* query = _defaultQueries.back();
    _defaultQueries.pop_back();
    return NavPathQueryPtr(query, NavPathQueryDeleter(this));
}

void NavPathQueryPool::release(NavPathQuery* query)
{
    CK_ASSERT_RETURN(_defaultQueryPool.verify(query));
    
    _defaultQueries.emplace_back(query);
}
    
} /* namespace ove */
    
} /* namespace cinek */