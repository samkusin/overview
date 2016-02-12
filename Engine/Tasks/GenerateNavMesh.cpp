//
//  GenerateNavMesh.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#include "GenerateNavMesh.hpp"
#include "Engine/Contrib/Recast/DetourNavMeshBuilder.h"

#include "Engine/Debug.hpp"

namespace cinek {
    namespace ove {

const UUID GenerateNavMesh::kUUID = {
    0x8f, 0x2e, 0x5e, 0x30, 0x63, 0x20, 0x46, 0xb3,
    0x9e, 0x2a, 0x98, 0xf3, 0xa8, 0xa1, 0x9a, 0x16
};

    
GenerateNavMesh::GenerateNavMesh
(
    const RecastMeshConfig& config,
    recast_poly_mesh_unique_ptr polyMeshInput,
    recast_detail_mesh_unique_ptr detailMeshInput
) :
    _config(config),
    _pmesh(std::move(polyMeshInput)),
    _dmesh(std::move(detailMeshInput)),
    _navData(nullptr),
    _navDataSize(0),
    _stage(kInitialStage)
{
}

GenerateNavMesh::~GenerateNavMesh()
{
    if (_navData) {
        dtFree(_navData);
        _navData = nullptr;
    }
}
    
NavMesh GenerateNavMesh::acquireGeneratedMesh()
{
    CK_ASSERT(_stage == kFinalizeStage);
    return NavMesh(std::move(_navmesh));
}
    
    
void GenerateNavMesh::onUpdate(uint32_t deltaTimeMs)
{
    switch (_stage) {
    
    case kInitialStage:
        {
            dtNavMeshCreateParams params;
            
            memset(&params, 0, sizeof(params));
            // core config
            params.cs = _config.cellSize;
            params.ch = _config.cellHeight;
            params.walkableHeight = _config.walkableHeight;
            params.walkableClimb = _config.walkableClimb;
            params.walkableRadius = _config.walkableRadius;
            rcVcopy(params.bmin, _pmesh->bmin);
            rcVcopy(params.bmax, _pmesh->bmax);
            params.buildBvTree = true;
            
            //  mesh config
            params.verts = _pmesh->verts;
            params.vertCount = _pmesh->nverts;
            params.polys = _pmesh->polys;
            params.polyAreas = _pmesh->areas;
            params.polyFlags = _pmesh->flags;
            params.polyCount = _pmesh->npolys;
            params.nvp = _pmesh->nvp;
            
            //  optional set
            params.detailMeshes = _dmesh->meshes;
            params.detailVerts = _dmesh->verts;
            params.detailVertsCount = _dmesh->nverts;
            params.detailTris = _dmesh->tris;
            params.detailTriCount = _dmesh->ntris;
            
            if (!dtCreateNavMeshData(&params, &_navData, &_navDataSize)) {
                OVENGINE_LOG_ERROR("GenerateNavMesh - failed to create detour mesh!\n");
                fail();
                return;
            }
            
            _stage = kMeshStage;
        }
        break;
        
    case kMeshStage:
        {
            _navmesh = detour_nav_mesh_unique_ptr(dtAllocNavMesh());
            if (!_navmesh) {
                OVENGINE_LOG_ERROR("GenerateNavMesh - failed to allocate nav mesh.\n");
                fail();
                return;
            }
            dtStatus status = _navmesh->init(_navData, _navDataSize, DT_TILE_FREE_DATA);
            if (dtStatusFailed(status)) {
                OVENGINE_LOG_ERROR("GenerateNavMesh - failed to init nav mesh.\n");
                fail();
                return;
            }
            _navData = nullptr; // owned by navmesh
            _navDataSize = 0;
            _stage = kFinalizeStage;
        }
        break;
        
    case kFinalizeStage:
        end();
        break;
    
    }
}
    
    } /* namesapce ove */
} /* namespace cinek */