//
//  Animation.hpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/22/15.
//
//

#ifndef CK_Graphics_Animation_hpp
#define CK_Graphics_Animation_hpp

#include "GfxTypes.hpp"
#include <vector>
#include <array>
#include <string>

namespace cinek {
    namespace gfx {

    struct Keyframe
    {
        enum Type
        {
            kTranslateX,
            kTranslateY,
            kTranslateZ,
            kQuaternionW,
            kQuaternionX,
            kQuaternionY,
            kQuaternionZ,
            kScaleX,
            kScaleY,
            kScaleZ,
            kTypeCount,
            kInvalid = -1
        };
        
        float t;
        float v;
    };
    
    using Sequence = std::vector<Keyframe>;
    using SequenceChannel = std::array<Sequence, Keyframe::kTypeCount>;
    
    struct Animation
    {
        std::vector<SequenceChannel> channels;
        float duration;
    };
    
    struct Bone
    {
        Matrix4 mtx;
        std::string name;
        int parent;
        int firstChild;
        int nextSibling;
        
        Bone() : parent(-1), firstChild(-1), nextSibling(-1) {}
    };
    
    struct AnimationSet
    {
        CK_CLASS_NON_COPYABLE(AnimationSet);
        
        AnimationSet() {}
        AnimationSet(AnimationSet&& other);
        AnimationSet& operator=(AnimationSet&& other);
        
        std::vector<Bone> bones;
        std::vector<std::pair<std::string, Animation>> animations;
        
        void add(Animation&& animation, const std::string& name);
        const Animation* find(const std::string& name) const;
        int findBoneIndex(const std::string& name) const;
    };
    

    }   // namespace gfx
}   // namespace cinek

#endif /* Animation_hpp */
