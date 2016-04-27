//
//  RecastMesh.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#ifndef Overview_Nav_RecastMesh_hpp
#define Overview_Nav_RecastMesh_hpp

#include "PathTypes.hpp"
#include "Engine/Contrib/Recast/Recast.h"

#include <cinek/allocator.hpp>
#include <vector>


namespace cinek {
    namespace ove {

    struct recast_poly_mesh_deleter
    {
        void operator()(rcPolyMesh* ptr)  {
            rcFreePolyMesh(ptr);
        }
    };
    struct recast_detail_mesh_deleter
    {
        void operator()(rcPolyMeshDetail* ptr)  {
            rcFreePolyMeshDetail(ptr);
        }
    };
    
    using recast_poly_mesh_unique_ptr = std::unique_ptr<rcPolyMesh, recast_poly_mesh_deleter>;
    using recast_detail_mesh_unique_ptr = std::unique_ptr<rcPolyMeshDetail, recast_detail_mesh_deleter>;
    
    struct recast_heighfield_deleter
    {
        void operator()(rcHeightfield* ptr)  {
            rcFreeHeightField(ptr);
        }
    };
    struct recast_compact_heightfield_deleter
    {
        void operator()(rcCompactHeightfield* ptr)  {
            rcFreeCompactHeightfield(ptr);
        }
    };
    struct recast_contour_set_deleter
    {
        void operator()(rcContourSet* ptr)  {
            rcFreeContourSet(ptr);
        }
    };
        
    using recast_heighfield_unique_ptr = std::unique_ptr<rcHeightfield, recast_heighfield_deleter>;
    using recast_compact_heightfield_unique_ptr = std::unique_ptr<rcCompactHeightfield, recast_compact_heightfield_deleter>;
    using recast_contour_set_unique_ptr = std::unique_ptr<rcContourSet, recast_contour_set_deleter>;
    
    struct RecastMeshInput
    {        
        //  input mesh data for the recast mesh generator
        std::vector<float> vertexData;
        std::vector<int> triangleData;
        float bmin[3];
        float bmax[3];
        int numTriangles;
        int numVertices;

        RecastMeshInput() { reset(); }

        void reset()
        {
            numTriangles = 0;
            numVertices = 0;
            vertexData.clear();
            triangleData.clear();
            bmin[0] = bmin[1] = bmin[2] = 0.0f;
            bmax[0] = bmax[1] = bmax[2] = 0.0f;
        }
    };
    
    class RecastMesh
    {
        CK_CLASS_NON_COPYABLE(RecastMesh);
        
    public:
        RecastMesh() = default;
        
        RecastMesh(recast_poly_mesh_unique_ptr pmesh, recast_detail_mesh_unique_ptr dmesh);
        
        RecastMesh
        (
            recast_heighfield_unique_ptr hf,
            recast_compact_heightfield_unique_ptr chf,
            recast_contour_set_unique_ptr cset,
            recast_poly_mesh_unique_ptr pmesh,
            recast_detail_mesh_unique_ptr dmesh
        );
        RecastMesh(RecastMesh&& other);
        RecastMesh& operator=(RecastMesh&& other);
        
        recast_poly_mesh_unique_ptr acquirePolyMesh();
        recast_detail_mesh_unique_ptr acquireDetailMesh();
        
        void debugDraw(::duDebugDraw& debugDraw);
        
    private:
        recast_heighfield_unique_ptr _hf;
        recast_compact_heightfield_unique_ptr _chf;
        recast_contour_set_unique_ptr _cset;
        
        recast_poly_mesh_unique_ptr _pmesh;
        recast_detail_mesh_unique_ptr _dmesh;
    };

    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_Nav_RecastMesh_hpp */
