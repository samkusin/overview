//
//  NavMesh.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#ifndef Overview_Path_NavMesh_hpp
#define Overview_Path_NavMesh_hpp

#include "PathTypes.hpp"
#include "Engine/Contrib/Recast/DetourNavMesh.h"

#include "RecastMesh.hpp"

#include <cinek/allocator.hpp>

struct dtNavMeshCreateParams;

namespace cinek {
    namespace ove {
 
    struct detour_nav_mesh_deleter
    {
        void operator()(dtNavMesh* ptr) {
            dtFreeNavMesh(ptr);
        }
    };
    
    using detour_nav_mesh_unique_ptr = std::unique_ptr<dtNavMesh, detour_nav_mesh_deleter>;
    
    
    class NavMesh
    {
        CK_CLASS_NON_COPYABLE(NavMesh);
        
    public:
        NavMesh() = default;
        NavMesh(detour_nav_mesh_unique_ptr mesh);
        NavMesh(NavMesh&& other);
        NavMesh& operator=(NavMesh&& other);
        
        
        void debugDraw(::duDebugDraw& debugDraw);
        
    private:
        detour_nav_mesh_unique_ptr _mesh;
    };
    
    
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Path_NavMesh_hpp */
