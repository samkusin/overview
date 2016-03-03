//
//  AnimationController.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 11/2/15.
//
//

#ifndef AnimationController_hpp
#define AnimationController_hpp

#include "GfxTypes.hpp"

namespace cinek {
    namespace gfx {

class AnimationController
{
    CK_CLASS_NON_COPYABLE(AnimationController);
    
public:
    AnimationController();
    AnimationController(AnimationSetHandle animSet);
    
    AnimationController(AnimationController&& other);
    AnimationController& operator=(AnimationController&& other);
    
    void transitionToState(const char* stateName);
    
    float animationTime() const;
    const Animation* animation() const;
    
    void update(CKTime t);
    
private:
    AnimationSetHandle _animSet;
    const Animation* _thisAnim;
    float _time;
};

inline float AnimationController::animationTime() const {
    return _time;
}

inline const Animation* AnimationController::animation() const {
    return _thisAnim;
}

   
    }   // namespace gfx
}   // namespace cinek

#endif /* AnimationController_hpp */
