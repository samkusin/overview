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
        ckm::vector3f endPos
    );
    
    virtual ~GenerateNavPath();
    
    Entity entity() const { return _entity; }
    
    //  caller will acquire the list of points, and the task's list is cleared.
    std::vector<float> acquirePoints();
    
private:
    virtual void onUpdate(uint32_t /* deltaTimeMs */) override;
    
private:
    NavPathQueryPtr _query;
    Entity _entity;
    ckm::vector3f _startPos;
    ckm::vector3f _endPos;
    std::vector<float> _path;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* GenerateNavPath_hpp */
