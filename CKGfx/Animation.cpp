//
//  Animation.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/22/15.
//
//

#include "Animation.hpp"
#include <algorithm>

namespace cinek {
    namespace gfx {

        
    const Animation* AnimationSet::find(const std::string& name) const
    {
        auto it = std::lower_bound(animations.begin(), animations.end(), name,
            [](const std::pair<std::string, Animation>& anim, const std::string& name) {
                return anim.first < name;
            });
        if (it == animations.end())
            return nullptr;
        
        return &it->second;
    }
    

    void AnimationSet::add(Animation&& animation, const std::string& name)
    {
        auto it = std::lower_bound(animations.begin(), animations.end(), name,
            [](const std::pair<std::string, Animation>& anim, const std::string& name) {
                return anim.first < name;
            });
        if (it != animations.end() && it->first == name) {
            it->second = std::move(animation);
        }
        else {
            animations.emplace(it, name, std::move(animation));
        }
    }
    
    int AnimationSet::findBoneIndex(const std::string& name) const
    {
        auto it = std::find_if(bones.begin(), bones.end(),
            [&name](const Bone& b) -> bool {
                return b.name == name;
            });
        if (it == bones.end())
            return -1;
        
        return (int)(it - bones.begin());
    }
    
    AnimationSet::AnimationSet(AnimationSet&& other) :
        bones(std::move(other.bones)),
        animations(std::move(other.animations))
    {
    }
    
    AnimationSet& AnimationSet::operator=(AnimationSet&& other)
    {
        bones = std::move(other.bones);
        animations = std::move(other.animations);
        return *this;
    }
        
    }   // namespace gfx
}   // namespace cinek