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
        
        bool operator<(float ot) const {
            return t < ot;
        }
    };
    
    using Sequence = std::vector<Keyframe>;
    
    struct SequenceChannel
    {
        std::array<Sequence, Keyframe::kTypeCount> sequences;
        uint32_t animatedSeqMask = 0;
        
        auto keyframePairFromTime(Keyframe::Type kfType, float t) const ->
            std::pair<const Keyframe*, const Keyframe*>;
    };
    
    struct Animation
    {
        std::vector<SequenceChannel> channels;
        float duration = 0.f;
    };
    
    struct Bone
    {
        Matrix4 mtx;        // armature space matrix
        Matrix4 invMtx;     // inverse (armature to bone space)
        
        std::string name;
        int parent = -1;
        int firstChild = -1;
        int nextSibling = -1;
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
    
    
    void interpRotationFromSequenceChannel
    (
        Vector4& boneRotQuat,
        const SequenceChannel& channel,
        float animTime
    );
    
    void interpTranslateFromSequenceChannel
    (
        Vector3& translate,
        const SequenceChannel& channel,
        float animTime
    );
    

    }   // namespace gfx
}   // namespace cinek

#endif /* Animation_hpp */
