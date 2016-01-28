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
            kRotationX,
            kRotationY,
            kRotationZ,
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
    
    using Sequence = std::vector<Keyframe, std_allocator<Keyframe>>;
    
    struct SequenceChannel
    {
        std::array<Sequence, Keyframe::kTypeCount> sequences;
        uint32_t animatedSeqMask = 0;
        
        auto keyframePairFromTime(Keyframe::Type kfType, float t) const ->
            std::pair<const Keyframe*, const Keyframe*>;
        
        bool hasQuaternions() const {
            return !sequences[Keyframe::kQuaternionW].empty();
        }
        bool hasEulers() const {
            return !sequences[Keyframe::kRotationX].empty();
        }
    };
    
    struct Animation
    {
        std::vector<SequenceChannel, std_allocator<SequenceChannel>> channels;
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
        
        using StateDefinition = std::pair<std::string, Animation>;
        
        AnimationSet() {}
        AnimationSet(std::vector<Bone, std_allocator<Bone>>&& bones,
                     std::vector<StateDefinition, std_allocator<StateDefinition>>&& states);
        AnimationSet(AnimationSet&& other);
        AnimationSet& operator=(AnimationSet&& other);
        
        const Animation* find(const char* name) const;
        int findBoneIndex(const char* name) const;
        const Bone* boneFromIndex(int index) const;
        int boneCount() const { return (int)_bones.size(); }
        
    private:
        std::vector<StateDefinition, std_allocator<StateDefinition>> _animations;
        std::vector<Bone, std_allocator<Bone>> _bones;
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
    
    void interpScaleFromSequenceChannel
    (
        Vector3& scale,
        const SequenceChannel& channel,
        float animTime
    );
    

    }   // namespace gfx
}   // namespace cinek

#endif /* Animation_hpp */
