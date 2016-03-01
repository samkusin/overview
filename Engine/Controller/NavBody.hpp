//
//  NavBody.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/16/16.
//
//

#ifndef Overview_NavBody_hpp
#define Overview_NavBody_hpp

#include "ControllerTypes.hpp"

#include "Engine/Path/NavPath.hpp"

#include <vector>

namespace cinek {
    namespace ove {
    
class NavBody
{
    CK_CLASS_NON_COPYABLE(NavBody);
    
public:
    struct InitProperties
    {
        ckm::scalar speedLimit;
        Entity entity;
        
        InitProperties() : entity(0) {}
    };
    
    NavBody(InitProperties initProperties=InitProperties());
    NavBody(const NavBody& source, Entity entity);
    
    void setTransform(NavBodyTransform* transform);
    void pullTransform();
    void pushTransformPosOrient(ckm::vector3f position, ckm::matrix3f orient);
    void pushTransformVelocity(ckm::vector3f linear, ckm::vector3f angular);
    
    //  general properties
    Entity entity() const { return _entity; }
    const ckm::vector3f& position() const { return _position; }
    const ckm::matrix3f& basis() const { return _basis; }
   
    enum class State
    {
        kIdle,
        kPathStart,
        kPathRun,
        kPathBreak,
        kPathEnd
    };
    State state() const { return _state; }
    
    //  pathfinding
    void setPath(NavPath&& path);
    void runPath();
    const NavPath& currentPath() const { return _path; }
    State updatePath(dtPolyRef currentPoly);
    void setToIdle();

    //  steering
    void setSpeedScalar(ckm::scalar speed);
    ckm::scalar speedScalar() const { return _speed; }
    ckm::scalar calcAbsoluteSpeed() const;
    
private:
    //  Framework properties
    Entity _entity;
    ckm::vector3f _position;
    ckm::matrix3f _basis;
    NavBodyTransform* _transform;
    State _state;

    //  Steering properties
    NavPath _path;
    ckm::scalar _speedLimit;
    ckm::scalar _speed;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_NavBody_hpp */
