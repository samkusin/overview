//
//  PathTypes.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#ifndef Overview_Nav_PathTypes_hpp
#define Overview_Nav_PathTypes_hpp

#include "Engine/EngineTypes.hpp"

#include <cinek/allocator.hpp>

struct duDebugDraw;
struct dtNavMeshQuery;
struct dtNavMesh;

namespace cinek {
    namespace ove {
        
struct RecastMeshConfig
{
    float walkableClimb;
    float walkableHeight;
    float walkableRadius;
    float cellSize;
    float cellHeight;
};

enum
{
    kNavMeshPoly_Walkable   = 0x0001
};

struct detour_nav_mesh_deleter { void operator()(dtNavMesh* ptr); };
using detour_nav_mesh_unique_ptr = std::unique_ptr<dtNavMesh, detour_nav_mesh_deleter>;

struct detour_nav_query_deleter { void operator()(dtNavMeshQuery* ptr); };
using detour_nav_query_unique_ptr = std::unique_ptr<dtNavMeshQuery, detour_nav_query_deleter>;

struct NavPathQueryDeleter
{
    NavPathQueryDeleter() : owner(nullptr) {}
    NavPathQueryDeleter(NavPathQueryPool* pool) : owner(pool) {}
    
    void operator()(NavPathQuery* ptr);

    NavPathQueryPool* owner;
};
using NavPathQueryPtr = std::unique_ptr<NavPathQuery, NavPathQueryDeleter>;

    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_PathTypes_hpp */
