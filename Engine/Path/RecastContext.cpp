//
//  RecastContext.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#include "RecastContext.hpp"
#include "Engine/Debug.hpp"

namespace cinek {
    namespace ove {
    
    RecastContext::RecastContext()
    {
    }
        
    void RecastContext::doLog
    (
        const rcLogCategory category,
        const char* msg,
        const int /*len*/
    )
    {
        if (category == RC_LOG_PROGRESS) {
            OVENGINE_LOG_INFO(msg);
        }
        else if (category == RC_LOG_WARNING) {
            OVENGINE_LOG_WARN(msg);
        }
        else if (category == RC_LOG_ERROR) {
            OVENGINE_LOG_ERROR(msg);
        }
    }

    
    RecastNavMesh::RecastNavMesh
    (
        recast_poly_mesh_unique_ptr pmesh,
        recast_detail_mesh_unique_ptr dmesh
    ) :
        _pmesh(std::move(pmesh)),
        _dmesh(std::move(dmesh))
    {
    }
    
    RecastNavMesh::RecastNavMesh(RecastNavMesh&& other) :
        _pmesh(std::move(other._pmesh)),
        _dmesh(std::move(other._dmesh))
    {
    }
    
    RecastNavMesh& RecastNavMesh::operator=(RecastNavMesh&& other)
    {
        _pmesh = std::move(other._pmesh);
        _dmesh = std::move(other._dmesh);
        return *this;
    }
    
    } /* namespace ove */
} /* namespace cinek */