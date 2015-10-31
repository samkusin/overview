//
//  Animation.cpp
//  GfxPrototype
//
//  Created by Samir Sinha on 10/22/15.
//
//

#include "Animation.hpp"

#include <ckm/math.hpp>
#include <bx/fpumath.h>
#include <algorithm>
#include <cfloat>

namespace bx {
    inline void quatSlerp(float* __restrict _result, const float* __restrict _qa, const float* __restrict _qb, float _factor)
    {
        float cx,cy,cz,cw;
        
        const float ax = _qa[0];
        const float ay = _qa[1];
        const float az = _qa[2];
        const float aw = _qa[3];
        const float bx = _qb[0];
        const float by = _qb[1];
        const float bz = _qb[2];
        const float bw = _qb[3];
        
        float theta = ax * bx + ay * by + az * bz + aw * bw;
        if (theta < 0.0f) {
            cx = -bx;
            cy = -by;
            cz = -bz;
            cw = -bw;
            theta = -theta;
        }
        else {
            cx = bx;
            cy = by;
            cz = bz;
            cw = bw;
        }
        if (theta > 1.0f - FLT_EPSILON) {
            cx = flerp(ax,cx,_factor);
            cy = flerp(ay,cy,_factor);
            cz = flerp(az,cz,_factor);
            cw = flerp(aw,cw,_factor);
        }
        else {
            float angle = acosf(theta);
            float a = sin((1.0f - _factor) * angle);
            float c = sin(_factor*angle);
            float invsina = 1.0f/sinf(angle);
            cx = (a * ax + c * cx) * invsina;
            cy = (a * ay + c * cy) * invsina;
            cz = (a * az + c * cz) * invsina;
            cw = (a * aw + c * cw) * invsina;
        }
        
        //  normalize
        const float qcinvlen = 1.0f/fsqrt(cx * cx + cy * cy + cz * cz + cw * cw);
        _result[0] = cx * qcinvlen;
        _result[1] = cy * qcinvlen;
        _result[2] = cz * qcinvlen;
        _result[3] = cw * qcinvlen;
    }
}

namespace cinek {
    namespace gfx {


auto SequenceChannel::keyframePairFromTime
(
    Keyframe::Type kfType,
    float t
)
const -> std::pair<const Keyframe*, const Keyframe*>
{
    if (sequences[kfType].empty())
        return std::make_pair(nullptr, nullptr);
    
    auto it = std::lower_bound(sequences[kfType].begin(), sequences[kfType].end(), t);
    Sequence::const_iterator prevIt;
    
    if (it == sequences[kfType].end()) {
        prevIt = --it;
    }
    else if (it == sequences[kfType].begin()) {
        prevIt = it;
    }
    else {
        prevIt = it-1;
    }
    
    return std::make_pair(&(*prevIt), &(*it));
}
        
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


////////////////////////////////////////////////////////////////////////////////



void interpRotationFromSequenceChannel
(
    Vector4& boneRotQuat,
    const SequenceChannel& channel,
    float animTime
)
{
    Vector4 startQ;
    Vector4 endQ;
    float dt = 0.0f;
    float start = 0.0f;

    //  keyframe pairs for each sequence are not guaranteed to be aligned by
    //  time.  this means that we must calculate the delta time between two
    //  quaternions using the qx,qy,qz,qw keyframe pairs matching the given
    //  animation time on this channel.
    //
    auto kf = channel.keyframePairFromTime(Keyframe::kQuaternionX, animTime);
    if (!kf.first) return;
    startQ.x = kf.first->v;
    endQ.x = kf.second->v;
    float kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;

    kf = channel.keyframePairFromTime(Keyframe::kQuaternionY, animTime);
    if (!kf.first) return;
    startQ.y = kf.first->v;
    endQ.y = kf.second->v;
    kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;
    
    kf = channel.keyframePairFromTime(Keyframe::kQuaternionZ, animTime);
    if (!kf.first) return;
    startQ.z = kf.first->v;
    endQ.z = kf.second->v;
    kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;

    kf = channel.keyframePairFromTime(Keyframe::kQuaternionW, animTime);
    if (!kf.first) return;
    startQ.w = kf.first->v;
    endQ.w = kf.second->v;
    kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;
    
    if (dt < ckm::epsilon<float>())
        return;
    
    float factor = (animTime - start)/dt;
    if (factor < 0.0f)
        factor = 0.0f;
    else if (factor > 1.0f)
        factor = 1.0f;
    bx::quatSlerp(boneRotQuat, startQ, endQ, factor);
}


void interpTranslateFromSequenceChannel
(
    Vector3& translate,
    const SequenceChannel& channel,
    float animTime
)
{
}
        
    }   // namespace gfx
}   // namespace cinek