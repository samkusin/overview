//
//  NavMesh.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#include "NavMesh.hpp"
#include "Engine/Contrib/Recast/DebugDraw.h"
#include "Engine/Contrib/Recast/DetourDebugDraw.h"

namespace cinek {
    namespace ove {
    
NavMesh::NavMesh(detour_nav_mesh_unique_ptr mesh) :
    _mesh(std::move(mesh))
{
}

NavMesh::NavMesh(NavMesh&& other) :
    _mesh(std::move(other._mesh))
{
}


NavMesh& NavMesh::operator=(NavMesh&& other)
{
    _mesh = std::move(other._mesh);
    return *this;
}

void NavMesh::debugDraw(::duDebugDraw& debugDraw)
{
    if (_mesh) {
        debugDraw.depthMask(true);
    
        duDebugDrawNavMesh(&debugDraw, *_mesh, DU_DRAWNAVMESH_COLOR_TILES);
    
        debugDraw.depthMask(false);
    }
}
    

    
    } /* namespace ove */
} /* namespace cinek */
