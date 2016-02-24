//
//  NavPathQuery.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#include "NavPathQuery.hpp"
#include "NavMesh.hpp"
#include "NavPath.hpp"

#include "Engine/Contrib/Recast/DetourNavMeshQuery.h"
#include "Engine/Contrib/Recast/DetourNode.h"

#include <ckm/math.hpp>

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

float* NavPathQuery::PointList::getPoint(int index)
{
    return &points[index*3];
}

int NavPathQuery::plotPath
(
    PointList& outPointList,
    const NavPath& path,
    const ckm::vector3f& pos,
    ckm::scalar dist
)
const
{
    int resultCount = 0;
    interface().findStraightPath(pos.comp, path.targetPos().comp,
        path.polys(), path.numPolys(),
        outPointList.points, outPointList.flags, outPointList.polys,
        &resultCount, outPointList.size);
    
    if (!resultCount)
        return -1;
        
    int resultIndex = 0;
    while (resultIndex < resultCount) {
        auto points = &outPointList.points[resultIndex*3];
        auto flags = &outPointList.flags[resultIndex];
        
        ckm::vector3f delta(points[0], points[1], points[2]);
        ckm::sub(delta, delta, pos);
        auto testDist = ckm::vectorLength(delta);
        if ((*flags & DT_STRAIGHTPATH_OFFMESH_CONNECTION) || testDist >= dist)
            break;
    
        ++resultIndex;
    }
    return resultIndex;
}

  
    } /* namespace ove */
} /* namespace cinek */
