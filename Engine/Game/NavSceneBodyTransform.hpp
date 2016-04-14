//
//  NavSceneBodyTransform.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/16/16.
//
//

#ifndef Game_NavSceneBodyTransform_hpp
#define Game_NavSceneBodyTransform_hpp

#include "Engine/Controller/NavBodyTransform.hpp"

namespace cinek {
    namespace ove {
    
class NavSceneBodyTransform : public ove::NavBodyTransform
{
public:
    NavSceneBodyTransform(SceneBody* body);
    
    virtual void getNavBodyTransform
    (
        ckm::quat& basis,
        ckm::vector3& translate
    )
    override;
    
    virtual void setNavBodyTransform
    (
        ckm::quat basis,
        ckm::vector3 translate
    )
    override;
    
    virtual void setNavBodyVelocity
    (
        ckm::vector3 linear,
        ckm::vector3 angular
    )
    override;

private:
    SceneBody* _sceneBody;
};
    } /* namespace ove */
} /* namespace cinek */

#endif /* Game_NavSceneBodyTransform_hpp */
