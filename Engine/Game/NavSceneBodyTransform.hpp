//
//  NavSceneBodyTransform.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/16/16.
//
//

#ifndef Game_NavSceneBodyTransform_hpp
#define Game_NavSceneBodyTransform_hpp

#include "Engine/Nav/NavBodyTransform.hpp"

namespace cinek {
    namespace ove {
    
class NavSceneBodyTransform : public ove::NavBodyTransform
{
public:
    NavSceneBodyTransform(SceneBody* body);
    
    virtual void getNavBodyTransform
    (
        ckm::vector3f& translate,
        ckm::matrix3f& basis
    )
    override;
    
    virtual void setNavBodyTransform
    (
        ckm::vector3f translate,
        ckm::matrix3f basis
    )
    override;
    
    virtual void setNavBodyVelocity
    (
        ckm::vector3f linear,
        ckm::vector3f angular
    )
    override;

private:
    SceneBody* _sceneBody;
};
    } /* namespace ove */
} /* namespace cinek */

#endif /* Game_NavSceneBodyTransform_hpp */
