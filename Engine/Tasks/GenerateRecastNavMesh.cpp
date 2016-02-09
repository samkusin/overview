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
    const SceneFixedBodyHull& hull,
    const btTransform& transform,
    std::function<void(RecastNavMesh&&)> cb
) :
    Task(),     // callback defined in our constructor body
    _solid(nullptr),
    _chf(nullptr),
    _cset(nullptr),
    _polymesh(nullptr),
    _detailmesh(nullptr),
    _stage(kInitialStage),
    _callback(std::move(cb))
{
    //  generate Recast ingestible vertices from our source hull data
    //  a transform is supplied in case we're generating a nav-mesh from local
    //  hull data
    
    const float* vertexData = hull.vertexData();
    const int* indexData = hull.indexData();
    
    _vertexData.reserve(hull.vertexCount() * 3);
    
    memset(&_config, 0, sizeof(_config));
    
    float* bmin = _config.bmin;
    float* bmax = _config.bmax;
    
    for (int vidx = 0; vidx < hull.vertexCount(); ++vidx) {
        btVector3 btv;
        btv.setValue(vertexData[0], vertexData[1], vertexData[2]);
        btv = transform.getBasis() * btv;
        btv += transform.getOrigin();
        
        _vertexData.push_back(btv.getX());
        _vertexData.push_back(btv.getY());
        _vertexData.push_back(btv.getZ());
        
        if (btv.getX() < bmin[0]) {
            bmin[0] = btv.getX();
        }
        if (btv.getY() < bmin[1]) {
            bmin[1] = btv.getY();
        }
        if (btv.getZ() < bmin[2]) {
            bmin[2] = btv.getZ();
        }
        if (btv.getX() > bmax[0]) {
            bmax[0] = btv.getX();
        }
        if (btv.getY() < bmax[1]) {
            bmax[1] = btv.getY();
        }
        if (btv.getZ() < bmax[2]) {
            bmax[2] = btv.getZ();
        }
        vertexData += 3;
    }
    
    _indexData = std::move(std::vector<int>(indexData, indexData + hull.triangleCount() * 3));
    
    CK_ASSERT_RETURN(_vertexData.size() == hull.vertexCount() * 3);
    CK_ASSERT_RETURN((_indexData.size() == hull.triangleCount() * 3) && (_indexData.size() % 3) == 0);
    
    //  TODO - configure?
    const float kAgentHeight = 2.0f;            // 2m - placeholder
    const float kAgentRadius = 0.75f;           // radius of agent (cylinder)
    const float kAgentMaxClimb = 0.75f;         // ledge height
    const float kWalkableSlopeAngle = 45.0f;    // walkable slope (for stairways)
    
    _config.cs = 0.25f;
    _config.ch = 0.25f;
    _config.walkableSlopeAngle = kWalkableSlopeAngle;
    _config.walkableHeight = (int)(ceilf(kAgentHeight / _config.ch));
    _config.walkableClimb = (int)(floorf(kAgentMaxClimb / _config.ch));
    _config.walkableRadius = (int)(ceilf(kAgentRadius / _config.cs));
    _config.minRegionArea = 12;                 // remove small areas (cells)
    _config.mergeRegionArea = 20;               // merge small areas (cells) into larger when possible
    _config.detailSampleDist = 6.0f;
    _config.detailSampleMaxError = 1.0f;
    
    rcCalcGridSize(_config.bmin, _config.bmax, _config.cs,
        &_config.width, &_config.height);
    
    _triareas.resize(_indexData.size()/3);
    
    //  issue completion callback on task completion
    setCallback([this](Task::State state, Task&, void*) {
        RecastNavMesh mesh;
        if (state == Task::State::kEnded) {
            recast_poly_mesh_unique_ptr pmesh(_polymesh);
            recast_detail_mesh_unique_ptr dmesh(_detailmesh);
            
            mesh = std::move(RecastNavMesh(std::move(pmesh), std::move(dmesh)));
            _polymesh = nullptr;
            _detailmesh = nullptr;
        }
        _callback(std::move(mesh));
    });
    
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
            if (!rcBuildPolyMesh(&_context, *_cset, 3, *_polymesh)) {
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
        }
        break;
        
    default:
        OVENGINE_LOG_ERROR("GenerateRecastNavMesh - invalid stage %d", _stage);
        fail();
        break;
    }
}

    }
}