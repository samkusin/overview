//
//  NavBody.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/16/16.
//
//

#ifndef Overview_NavBody_hpp
#define Overview_NavBody_hpp

#include "PathTypes.hpp"

#include <vector>

namespace cinek {
    namespace ove {
    
struct SpeedCurve
{
    enum Type
    {
        kStep,
        kLinear
    };
    
    ckm::scalar min = ckm::scalar(0);
    ckm::scalar max = ckm::scalar(0);
    Type type = kStep;
};

    
class NavBody
{
    CK_CLASS_NON_COPYABLE(NavBody);
    
public:
    struct InitProperties
    {
        SpeedCurve speedCurve;
        Entity entity;
        
        InitProperties() : entity(0) {}
    };
    
    NavBody(InitProperties initProperties=InitProperties());
    NavBody(const NavBody& source, Entity entity);
    
    void setNavBodyTransform(NavBodyTransform* transform);
    
    Entity entity() const { return _entity; }
    
private:
    Entity _entity;
    NavBodyTransform* _transform;
    std::vector<ckm::vector3f> _path;
    SpeedCurve _speedCurve;
};
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_NavBody_hpp */
