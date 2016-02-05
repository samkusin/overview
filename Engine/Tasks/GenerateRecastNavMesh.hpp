//
//  GenerateRecastNavMesh.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#ifndef Overview_Task_GenerateRecastNavMesh_hpp
#define Overview_Task_GenerateRecastNavMesh_hpp


#include <cinek/task.hpp>

#include "Engine/Scenes/SceneTypes.hpp"
#include "Engine/Path/RecastContext.hpp"
#include "Engine/Contrib/Recast/Recast.h"

#include <vector>

namespace cinek {
    namespace ove {

class GenerateRecastNavMesh : public Task
{
public:
    static const UUID kUUID;
    
    GenerateRecastNavMesh
    (
        const SceneFixedBodyHull& hull,
        const btTransform& transform,
        std::function<void(RecastNavMesh&&)> cb
    );
    
    virtual ~GenerateRecastNavMesh();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
    
    
protected:
    virtual void onUpdate(uint32_t deltaTimeMs) override;
    
private:
    std::vector<float> _vertexData;
    std::vector<int> _indexData;
    std::vector<unsigned char> _triareas;
    
    //  these objects are temporary - used for construction
    RecastContext _context;
    rcConfig _config;
    rcHeightfield* _solid;
	rcCompactHeightfield* _chf;
	rcContourSet* _cset;
    
    //  these objects are returned
	rcPolyMesh* _polymesh;
	rcPolyMeshDetail* _detailmesh;
    
    enum
    {
        kInitialStage,
        kRasterizeStage,
        kFilterStage,
        kPartitionStage,
        kContourStage,
        kFinalizeStage
    }
    _stage;
    
    std::function<void(RecastNavMesh&&)> _callback;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_GenerateRecastNavMesh_hpp */
