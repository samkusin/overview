//
//  GenerateNavPath.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/19/16.
//
//

#ifndef Overview_Task_GenerateNavPath_hpp
#define Overview_Task_GenerateNavPath_hpp

#include "Engine/Nav/PathTypes.hpp"
#include "Engine/Contrib/Recast/DetourNavMesh.h"

#include <cinek/task.hpp>
#include <vector>

namespace cinek {
    namespace ove {
    
class GenerateNavPath : public Task
{
public:
    GenerateNavPath
    (
        NavPathQueryPtr&& queryPtr,
        Entity entity,
        ckm::vector3f startPos,
        ckm::vector3f endPos,
        ckm::vector3f extents,
        const Allocator& allocator
    );
    
    virtual ~GenerateNavPath();
    
    Entity entity() const { return _entity; }
    ckm::vector3f startPos() const { return _startPos; }
    ckm::vector3f endPos() const { return _endPos; }
    
    //  caller will acquire the list of points, and the task's list is cleared.
    std::vector<dtPolyRef> acquirePoints();
    
private:
    virtual void onBegin() override;
    virtual void onUpdate(uint32_t /* deltaTimeMs */) override;
    
private:
    Allocator _allocator;
    NavPathQueryPtr _query;
    Entity _entity;
    ckm::vector3f _startPos;
    ckm::vector3f _endPos;
    ckm::vector3f _extents;
    dtPolyRef* _polyPath;
    int _polyPathIndex;
    int _polyPathSize;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* GenerateNavPath_hpp */
