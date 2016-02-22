//
//  NavPathQuery.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#include "NavPathQuery.hpp"
#include "NavMesh.hpp"

#include "Engine/Contrib/Recast/DetourNavMeshQuery.h"
#include "Engine/Contrib/Recast/DetourNode.h"

namespace cinek {
    namespace ove {

NavPathQuery::NavPathQuery(const NavMesh* mesh, int numNodes)
{
    detour_nav_query_unique_ptr ptr(dtAllocNavMeshQuery());
    ptr->init(mesh->detourMesh(), numNodes);
    _query = std::move(ptr);
}

int NavPathQuery::nodeLimit() const
{
    return _query->getNodePool()->getMaxNodes();
}

        
//  set's up node filters used during the next query
void NavPathQuery::setupFilters(uint16_t includeNodeMask)
{
    _filter.setIncludeFlags(includeNodeMask);
    _filter.setExcludeFlags(0);
}

//  is the point 'walkable' - i.e. on the walk mesh
bool NavPathQuery::isWalkable
(
    ckm::vector3f pos,
    ckm::vector3f extents
)
const
{
    dtPolyRef resultRef = 0;
    dtStatus status = _query->findNearestPoly(pos, extents, &_filter, &resultRef, nullptr);
    if (dtStatusFailed(status))
        return false;
    return resultRef != 0;
}

/*
std::vector<float> NavPathQuery::generatePath
(
    ckm::vector3f start,
    ckm::vector3f end
)
const
{
//m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);


}
*/
  
    } /* namespace ove */
} /* namespace cinek */
