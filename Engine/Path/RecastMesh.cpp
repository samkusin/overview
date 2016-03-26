//
//  NavMesh.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#include "RecastMesh.hpp"

#include "Engine/Contrib/Recast/RecastDebugDraw.h"
#include "Engine/Contrib/Recast/DebugDraw.h"

namespace cinek {
    namespace ove {

    RecastMesh::RecastMesh
    (
        recast_poly_mesh_unique_ptr pmesh,
        recast_detail_mesh_unique_ptr dmesh
    ) :
        _pmesh(std::move(pmesh)),
        _dmesh(std::move(dmesh))
    {
    }
    
    RecastMesh::RecastMesh
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
    
    RecastMesh::RecastMesh(RecastMesh&& other) :
        _hf(std::move(other._hf)),
        _chf(std::move(other._chf)),
        _cset(std::move(other._cset)),
        _pmesh(std::move(other._pmesh)),
        _dmesh(std::move(other._dmesh))
    {
    }
    
    RecastMesh& RecastMesh::operator=(RecastMesh&& other)
    {
        _hf = std::move(other._hf);
        _chf = std::move(other._chf);
        _cset = std::move(other._cset);
        _pmesh = std::move(other._pmesh);
        _dmesh = std::move(other._dmesh);
        return *this;
    }
    
    void RecastMesh::debugDraw(::duDebugDraw& debugDraw)
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
    
        
    recast_poly_mesh_unique_ptr RecastMesh::acquirePolyMesh()
    {
        return std::move(_pmesh);
    }
    
    recast_detail_mesh_unique_ptr RecastMesh::acquireDetailMesh()
    {
        return std::move(_dmesh);
    }

    } /* namespace ove */
} /* namespace cinek */