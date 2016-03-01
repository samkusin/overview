//
//  GenerateRecastMesh.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/4/16.
//
//

#ifndef Overview_Task_GenerateRecastMesh_hpp
#define Overview_Task_GenerateRecastMesh_hpp


#include "Engine/Physics/SceneTypes.hpp"
#include "Engine/Path/RecastContext.hpp"
#include "Engine/Path/RecastMesh.hpp"
#include "Engine/Contrib/Recast/Recast.h"

#include <cinek/task.hpp>


#include <vector>

namespace cinek {
    namespace ove {

class GenerateRecastMesh : public Task
{
public:
    static const UUID kUUID;
    
    GenerateRecastMesh(const RecastMeshConfig& config, RecastMeshInput input);
    
    virtual ~GenerateRecastMesh();
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
    enum
    {
        kOutputMesh             = 1,
        kHeightfield            = 2,
        kCompactHeightfield     = 4,
        kContourSet             = 8,
        kAllObjects             = kOutputMesh
                                  + kHeightfield
                                  + kCompactHeightfield
                                  + kContourSet
    };
    
    RecastMesh acquireGeneratedMesh(uint32_t options=kOutputMesh);
    
    
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
        kMeshStage,
        kFinalizeStage
    }
    _stage;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* Overview_Task_GenerateRecastMesh_hpp */
