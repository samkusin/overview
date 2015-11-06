//
//  AnimationController.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 11/2/15.
//
//

#include "AnimationController.hpp"
#include "Animation.hpp"

#include <cmath>

namespace cinek {
    namespace gfx {
    
AnimationController::AnimationController() :
    _thisAnim(nullptr),
    _time(0.0f)
{
}

AnimationController::AnimationController(AnimationSetHandle animSet) :
    _animSet(animSet),
    _thisAnim(nullptr),
    _time(0.0f)
{
}


AnimationController::AnimationController(AnimationController&& other) :
    _animSet(std::move(other._animSet)),
    _thisAnim(other._thisAnim),
    _time(other._time)
{
    other._thisAnim = nullptr;
}

AnimationController& AnimationController::operator=(AnimationController&& other)
{
    _animSet = std::move(other._animSet);
    _thisAnim = other._thisAnim;
    
    other._thisAnim = nullptr;
    
    return *this;
}
    
void AnimationController::transitionToState(const char* stateName)
{
    _thisAnim = _animSet->find(stateName);
    _time = 0.0f;
}

void AnimationController::update(double t)
{
    if (_thisAnim) {
        _time = fmod(t, _thisAnim->duration);
        /*
        if (_thisAnim->duration < 4.0f) {
            printf("%.4f = %.4f,%.4f\n", _time, t, _thisAnim->duration);
        }
        */
    }
}
   
    }   // namespace gfx
}   // namespace cinek