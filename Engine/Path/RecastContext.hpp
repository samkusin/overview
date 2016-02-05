//
//  RecastContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#ifndef Overview_Utils_RecastContext_hpp
#define Overview_Utils_RecastContext_hpp

#include "Engine/EngineTypes.hpp"
#include "Engine/Contrib/Recast/Recast.h"

#include <cinek/allocator.hpp>

namespace cinek {
    namespace ove {
    
    class RecastContext : public ::rcContext
    {
    public:
        RecastContext();
    
    protected:
    	virtual void doLog
        (
            const rcLogCategory category,
            const char* msg,
            const int /*len*/
        );
    };
    

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
    
    class RecastNavMesh
    {
        CK_CLASS_NON_COPYABLE(RecastNavMesh);
        
    public:
        RecastNavMesh() = default;
        RecastNavMesh(recast_poly_mesh_unique_ptr pmesh, recast_detail_mesh_unique_ptr dmesh);
        RecastNavMesh(RecastNavMesh&& other);
        RecastNavMesh& operator=(RecastNavMesh&& other);
        
    private:
        recast_poly_mesh_unique_ptr _pmesh;
        recast_detail_mesh_unique_ptr _dmesh;
    };

    } /* namespace ove */
} /* namespace cinek */

#endif /* RecastContext_hpp */
