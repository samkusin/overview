//
//  GenerateRecastNavMesh.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#include "GenerateRecastNavMesh.hpp"
#include "Engine/Physics/SceneFixedBodyHull.hpp"
#include "Engine/Debug.hpp"

#include <bullet/LinearMath/btTransform.h>

namespace cinek {
    namespace ove {
    
const UUID GenerateRecastNavMesh::kUUID = {
    0xd6, 0xc2, 0x79, 0x8c, 0x4e, 0x66, 0x47, 0x9e,
    0x9c, 0xb0, 0x94, 0x9d, 0x42, 0x5b, 0xeb, 0x35
};

GenerateRecastNavMesh::GenerateRecastNavMesh
(
    RecastNavMeshInput input
) :
    Task(),     // callback defined in our constructor body
    _vertexData(std::move(input.vertexData)),
    _indexData(std::move(input.triangleData)),
    _solid(nullptr),
    _chf(nullptr),
    _cset(nullptr),
    _polymesh(nullptr),
    _detailmesh(nullptr),
    _stage(kInitialStage)
{
    CK_ASSERT_RETURN((_vertexData.size() % 3) == 0);
    CK_ASSERT_RETURN((_indexData.size() % 3) == 0);
    
    //  TODO - configure?
    const float kAgentHeight = 2.0f;            // 2m - placeholder
    const float kAgentRadius = 0.1f;            // radius of agent (cylinder)
    const float kAgentMaxClimb = 0.5f;         // ledge height
    const float kWalkableSlopeAngle = 45.0f;    // walkable slope (for stairways)
    
    rcVcopy(_config.bmin, input.bmin);
    rcVcopy(_config.bmax, input.bmax);
    _config.cs = 0.20f;
    _config.ch = 0.025f;
    _config.walkableSlopeAngle = kWalkableSlopeAngle;
    _config.walkableHeight = (int)(ceilf(kAgentHeight / _config.ch));
    _config.walkableClimb = (int)(floorf(kAgentMaxClimb / _config.ch));
    _config.walkableRadius = (int)(ceilf(kAgentRadius / _config.cs));
    _config.minRegionArea = (int)rcSqr(5);      // remove small areas (cells)
    _config.mergeRegionArea = (int)rcSqr(10);   // merge small areas (cells) into larger when possible
    _config.detailSampleDist = _config.cs * 6.0f;
    _config.detailSampleMaxError = _config.ch * 1.0f;
    _config.maxEdgeLen = 12.0f/_config.cs;
    _config.maxSimplificationError = 1.0f;
    _config.maxVertsPerPoly = 6;
    
    rcCalcGridSize(_config.bmin, _config.bmax, _config.cs,
        &_config.width, &_config.height);
    
    _triareas.resize(_indexData.size()/3);
    
    _stage = kRasterizeStage;
}

GenerateRecastNavMesh::~GenerateRecastNavMesh()
{
    if (_detailmesh) {
        rcFreePolyMeshDetail(_detailmesh);
        _detailmesh = nullptr;
    }
    if (_polymesh) {
        rcFreePolyMesh(_polymesh);
        _polymesh = nullptr;
    }
    if (_cset) {
        rcFreeContourSet(_cset);
        _cset = nullptr;
    }
    if (_chf) {
        rcFreeCompactHeightfield(_chf);
        _chf = nullptr;
    }
    if (_solid) {
        rcFreeHeightField(_solid);
        _solid = nullptr;
    }
}

void GenerateRecastNavMesh::onUpdate(uint32_t deltaTimeMs)
{
    const float* vertices = _vertexData.data();
    const int* triangles = _indexData.data();
    
    const int numTris = (int)_indexData.size() / 3;
    const int numVerts = (int)_vertexData.size() / 3;
    
    switch (_stage) {
        case kRasterizeStage: {
            //  rasterization step - constructs voxel map
            if (!_config.width || !_config.height || _triareas.size() != numTris) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - setup failed.\n");
                fail();
                return;
            }
            
            //  recast - rasterize input mesh into voxel map for the partitioning
            //  stage
            _solid = rcAllocHeightfield();
            if (!_solid) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to allocate solid.\n");
                fail();
                return;
            }
            
            if (!rcCreateHeightfield(&_context, *_solid,
                    _config.width, _config.height,
                    _config.bmin, _config.bmax,
                    _config.cs, _config.ch)) {
                
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to create height field.\n");
                fail();
                return;
            }
        
            rcMarkWalkableTriangles(&_context, _config.walkableSlopeAngle,
                vertices, numVerts,
                triangles, numTris,
                _triareas.data());

            if (!rcRasterizeTriangles(&_context,
                    vertices, numVerts,
                    triangles, _triareas.data(), numTris,
                    *_solid, _config.walkableClimb)) {
                
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - rcRasterizeTriangles failed.\n");
                fail();
                return;
            }
            
            _stage = kFilterStage;
        }
        break;
        
        case kFilterStage: {
            //  apply various filters to the generated solid volume
            rcFilterLowHangingWalkableObstacles(&_context, _config.walkableClimb, *_solid);
            rcFilterLedgeSpans(&_context, _config.walkableHeight, _config.walkableClimb, *_solid);
            rcFilterWalkableLowHeightSpans(&_context, _config.walkableHeight, *_solid);
            
            _stage = kPartitionStage;
        }
        break;
        
        case kPartitionStage: {
            _chf = rcAllocCompactHeightfield();
            if (!_chf) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to allocate compact heightfield.\n");
                fail();
                return;
            }
            if (!rcBuildCompactHeightfield(&_context,
                _config.walkableHeight, _config.walkableClimb,
                *_solid, *_chf)) {
            
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to build compact heightfield.\n");
                fail();
                return;
            }
            if (!rcErodeWalkableArea(&_context, _config.walkableRadius, *_chf)) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to erode walkable area.\n");
                fail();
                return;
            }
            //  using watershed method until we find a use for other methods
            if (!rcBuildDistanceField(&_context, *_chf)) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to build distance field.\n");
                fail();
                return;
            }
            if (!rcBuildRegions(&_context, *_chf, 0, _config.minRegionArea, _config.mergeRegionArea)) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to build regions.\n");
                fail();
                return;
            }
            
            _stage = kContourStage;
        }
        break;
        
        case kContourStage: {
            _cset = rcAllocContourSet();
            if (!_cset) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to allocate contour set.\n");
                fail();
                return;
            }
            if (!rcBuildContours(&_context, *_chf,
                _config.maxSimplificationError, _config.maxEdgeLen,
                *_cset)) {
            
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to build contour set.\n");
                fail();
                return;
            }
            _stage = kFinalizeStage;
        }
        break;
        
        case kFinalizeStage: {
            _polymesh = rcAllocPolyMesh();
            if (!_polymesh) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to allocate poly mesh.\n");
                fail();
                return;
            }
            if (!rcBuildPolyMesh(&_context, *_cset, _config.maxVertsPerPoly, *_polymesh)) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to build poly mesh.\n");
                fail();
                return;
            }
            _detailmesh = rcAllocPolyMeshDetail();
            if (!_detailmesh) {
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to allocate poly detail mesh.\n");
                fail();
                return;
            }
            if (!rcBuildPolyMeshDetail(&_context,
                *_polymesh, *_chf,
                 _config.detailSampleDist, _config.detailSampleMaxError,
                 *_detailmesh)) {
                
                OVENGINE_LOG_ERROR("GenerateRecastNavMesh - failed to build poly detail mesh.\n");
                fail();
                return;
            }
            
            end();
        }
        break;
        
    default:
        OVENGINE_LOG_ERROR("GenerateRecastNavMesh - invalid stage %d", _stage);
        fail();
        break;
    }
}

void GenerateRecastNavMesh::onEnd()
{
    Task::onEnd();
}

RecastNavMesh GenerateRecastNavMesh::acquireGeneratedMesh(uint32_t options)
{
    RecastNavMesh navMesh;
    
    recast_poly_mesh_unique_ptr pmesh;
    recast_detail_mesh_unique_ptr dmesh;
    recast_heighfield_unique_ptr hf;
    recast_compact_heightfield_unique_ptr chf;
    recast_contour_set_unique_ptr cset;
    
    if ((options & kOutputMesh) != 0) {
        pmesh = recast_poly_mesh_unique_ptr(_polymesh);
        dmesh = recast_detail_mesh_unique_ptr(_detailmesh);
        _polymesh = nullptr;
        _detailmesh = nullptr;
    }
    if ((options & kHeightfield) != 0) {
        hf = recast_heighfield_unique_ptr(_solid);
        _solid = nullptr;
    }
    if ((options & kCompactHeightfield) != 0) {
        chf = recast_compact_heightfield_unique_ptr(_chf);
        _chf = nullptr;
    }
    if ((options & kContourSet) != 0) {
        cset = recast_contour_set_unique_ptr(_cset);
        _cset = nullptr;
    }
    
    if (options == kOutputMesh) {
        navMesh = std::move(RecastNavMesh(std::move(pmesh), std::move(dmesh)));
    }
    else {
        navMesh = std::move(RecastNavMesh(
            std::move(hf),
            std::move(chf),
            std::move(cset),
            std::move(pmesh),
            std::move(dmesh)
        ));
    }
    
    return navMesh;
}

    }
}