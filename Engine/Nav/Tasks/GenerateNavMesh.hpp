//
//  GenerateNavMesh.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/11/16.
//
//

#ifndef Overview_Task_GenerateNavMesh_hpp
#define Overview_Task_GenerateNavMesh_hpp

#include "Engine/Nav/NavMesh.hpp"

#include <cinek/task.hpp>

namespace cinek {
    namespace ove {
    
class GenerateNavMesh : public Task
{
public:
    static const UUID kUUID;
    
    GenerateNavMesh
    (
        const RecastMeshConfig& config,
        recast_poly_mesh_unique_ptr polyMeshInput,
        recast_detail_mesh_unique_ptr detailMeshInput
    );
    virtual ~GenerateNavMesh();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
    NavMesh acquireGeneratedMesh();
    
    
protected:
    virtual void onUpdate(uint32_t deltaTimeMs) override;
    
private:
    RecastMeshConfig _config;
    
    recast_poly_mesh_unique_ptr _pmesh;
    recast_detail_mesh_unique_ptr _dmesh;
    detour_nav_mesh_unique_ptr _navmesh;

    unsigned char* _navData;
    int _navDataSize;
    
    enum
    {
        kInitialStage,
        kMeshStage,
        kFinalizeStage
    }
    _stage;
};

    
    } /* namesapce ove */
} /* namespace cinek */

#endif /* Overview_Task_GenerateNavMesh_hpp */
