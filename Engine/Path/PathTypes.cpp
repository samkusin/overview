//
//  PathTypes.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#include "PathTypes.hpp"
#include "NavPathQuery.hpp"
#include "NavPathQueryPool.hpp"

#include "Engine/Contrib/Recast/DetourNavMesh.h"
#include "Engine/Contrib/Recast/DetourNavMeshQuery.h"

namespace cinek {

namespace ove {
 
void detour_nav_mesh_deleter::operator()(dtNavMesh* ptr)
{
    dtFreeNavMesh(ptr);
}

void detour_nav_query_deleter::operator()(dtNavMeshQuery* ptr)
{
    dtFreeNavMeshQuery(ptr);
}

void NavPathQueryDeleter::operator()(NavPathQuery* ptr)
{
    owner->release(ptr);
}
        
} /* namespace ove */

} /* namespace cinek */