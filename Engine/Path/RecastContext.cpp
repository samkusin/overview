//
//  RecastContext.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#include "RecastContext.hpp"
#include "Engine/Debug.hpp"

#include "Engine/Contrib/Recast/RecastDebugDraw.h"
#include "Engine/Contrib/Recast/DebugDraw.h"

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
    
    RecastNavMesh::RecastNavMesh
    (
        recast_heighfield_unique_ptr hf,
        recast_compact_heightfield_unique_ptr chf,
        recast_contour_set_unique_ptr cset,
        recast_poly_mesh_unique_ptr pmesh,
        recast_detail_mesh_unique_ptr dmesh
    ) :
        _hf(std::move(hf)),
        _chf(std::move(chf)),
        _cset(std::move(cset)),
        _pmesh(std::move(pmesh)),
        _dmesh(std::move(dmesh))
    {
    }
    
    RecastNavMesh::RecastNavMesh(RecastNavMesh&& other) :
        _hf(std::move(other._hf)),
        _chf(std::move(other._chf)),
        _cset(std::move(other._cset)),
        _pmesh(std::move(other._pmesh)),
        _dmesh(std::move(other._dmesh))
    {
    }
    
    RecastNavMesh& RecastNavMesh::operator=(RecastNavMesh&& other)
    {
        _hf = std::move(other._hf);
        _chf = std::move(other._chf);
        _cset = std::move(other._cset);
        _pmesh = std::move(other._pmesh);
        _dmesh = std::move(other._dmesh);
        return *this;
    }
    
    void RecastNavMesh::debugDraw(::duDebugDraw& debugDraw)
    {
        debugDraw.depthMask(true);
        
        if (_hf) {
            duDebugDrawHeightfieldSolid(&debugDraw, *_hf);
        }
        else if (_chf) {
            duDebugDrawCompactHeightfieldSolid(&debugDraw, *_chf);
        }
        else if (_cset) {
            duDebugDrawContours(&debugDraw, *_cset);
        }
        
        if (_dmesh) {
            duDebugDrawPolyMeshDetail(&debugDraw, *_dmesh);
        }
        else if (_pmesh) {
            duDebugDrawPolyMesh(&debugDraw, *_pmesh);
        }
        
        debugDraw.depthMask(false);
    }
    
    } /* namespace ove */
} /* namespace cinek */