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
    inline void slerp(float* __restrict _result, const float* __restrict _qa, const float* __restrict _qb, float _factor)
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
            float a = sinf((1.0f - _factor) * angle);
            float c = sinf(_factor*angle);
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
    
    inline void eulerToQuat(float* _result, const float *_euler) {
        const float ex_2 = _euler[0]*0.5f;
        const float ey_2 = _euler[1]*0.5f;
        const float ez_2 = _euler[2]*0.5f;
        const float cx = cosf(ex_2);
        const float cy = cosf(ey_2);
        const float cz = cosf(ez_2);
        const float sx = sinf(ex_2);
        const float sy = sinf(ey_2);
        const float sz = sinf(ez_2);
        _result[0] = cy*cz*sx - sy*sz*cx;
        _result[1] = cx*cz*sy + sx*sz*cy;
        _result[2] = cx*cy*sz - sx*sy*cz;
        _result[3] = cx*cy*cz + sx*sy*sz;
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
        
const Animation* AnimationSet::find(const char* name) const
{
    auto it = std::lower_bound(_animations.begin(), _animations.end(), name,
        [](const std::pair<std::string, Animation>& anim, const char* name) {
            return anim.first.compare(name) < 0;
        });
    if (it == _animations.end())
        return nullptr;
    
    return &it->second;
}

int AnimationSet::findBoneIndex(const char* name) const
{
    auto it = std::find_if(_bones.begin(), _bones.end(),
        [&name](const Bone& b) -> bool {
            return b.name == name;
        });
    if (it == _bones.end())
        return -1;
    
    return (int)(it - _bones.begin());
}

const Bone* AnimationSet::boneFromIndex(int index) const
{
    CK_ASSERT_RETURN_VALUE(index >= 0 && index < (int)_bones.size(), nullptr);
    return &_bones[index];
}
        

AnimationSet::AnimationSet(AnimationSet&& other) :
    _bones(std::move(other._bones)),
    _animations(std::move(other._animations))
{
}

AnimationSet& AnimationSet::operator=(AnimationSet&& other)
{
    _bones = std::move(other._bones);
    _animations = std::move(other._animations);
    return *this;
}

AnimationSet::AnimationSet
(
    std::vector<Bone, std_allocator<Bone>>&& bones,
    std::vector<StateDefinition, std_allocator<StateDefinition>>&& states
) :
    _bones(std::move(bones)),
    _animations(std::move(states))
{
    std::sort(_animations.begin(), _animations.end(),
        [](const StateDefinition& a0, const StateDefinition& a1) {
            return a0.first < a1.first;
        });
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


    //  process euler or quaternion values
    if (channel.hasQuaternions()) {
        //  keyframe pairs for each sequence are not guaranteed to be aligned by
        //  time.  this means that we must calculate the delta time between two
        //  quaternions using the qx,qy,qz,qw keyframe pairs matching the given
        //  animation time on this channel.
        //
        auto kf = channel.keyframePairFromTime(Keyframe::kQuaternionX, animTime);
        //if (!kf.first) return;
        //printf("qx[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
        startQ.x = kf.first->v;
        endQ.x = kf.second->v;
        float kfDt = kf.second->t - kf.first->t;
        if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
            dt = kfDt;
        if (kf.first->t > start)
            start = kf.first->t;

        kf = channel.keyframePairFromTime(Keyframe::kQuaternionY, animTime);
        //if (!kf.first) return;
        //printf("qy[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
        startQ.y = kf.first->v;
        endQ.y = kf.second->v;
        kfDt = kf.second->t - kf.first->t;
        if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
            dt = kfDt;
        if (kf.first->t > start)
            start = kf.first->t;

        kf = channel.keyframePairFromTime(Keyframe::kQuaternionZ, animTime);
        //if (!kf.first) return;
        //printf("qz[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
        startQ.z = kf.first->v;
        endQ.z = kf.second->v;
        kfDt = kf.second->t - kf.first->t;
        if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
            dt = kfDt;
        if (kf.first->t > start)
            start = kf.first->t;

        kf = channel.keyframePairFromTime(Keyframe::kQuaternionW, animTime);
        //if (!kf.first) return;
        //printf("qw[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
        startQ.w = kf.first->v;
        endQ.w = kf.second->v;
        kfDt = kf.second->t - kf.first->t;
        if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
            dt = kfDt;
        if (kf.first->t > start)
            start = kf.first->t;
    }
    else if (channel.hasEulers()) {
        // convert to quaternion -
        //
        // TODO, perhaps we should do this when ingesting the animation instead
        // of per frame!?!  That way we don't need to store euler keyframes and
        // we cut the need for this if quat else euler case.
        //
        Vector3 startR;
        Vector3 endR;
        
        //  keyframe pairs for each sequence are not guaranteed to be aligned by
        //  time.  this means that we must calculate the delta time between two
        //  quaternions using the qx,qy,qz,qw keyframe pairs matching the given
        //  animation time on this channel.
        //
        auto kf = channel.keyframePairFromTime(Keyframe::kRotationX, animTime);
        startR.x = kf.first->v;
        endR.x = kf.second->v;
        float kfDt = kf.second->t - kf.first->t;
        if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
            dt = kfDt;
        if (kf.first->t > start)
            start = kf.first->t;

        kf = channel.keyframePairFromTime(Keyframe::kRotationY, animTime);
        startR.y = kf.first->v;
        endR.y = kf.second->v;
        kfDt = kf.second->t - kf.first->t;
        if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
            dt = kfDt;
        if (kf.first->t > start)
            start = kf.first->t;

        kf = channel.keyframePairFromTime(Keyframe::kRotationZ, animTime);
        startR.z = kf.first->v;
        endR.z = kf.second->v;
        kfDt = kf.second->t - kf.first->t;
        if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
            dt = kfDt;
        if (kf.first->t > start)
            start = kf.first->t;
        
        bx::eulerToQuat(startQ, startR);
        bx::eulerToQuat(endQ, endR);
    }

    if (dt < ckm::epsilon<float>())
        return;
    
    float factor = (animTime - start)/dt;
    if (factor < 0.0f)
        factor = 0.0f;
    else if (factor > 1.0f)
        factor = 1.0f;
        
    bx::slerp(boneRotQuat, startQ, endQ, factor);
}


void interpTranslateFromSequenceChannel
(
    Vector3& translate,
    const SequenceChannel& channel,
    float animTime
)
{
    Vector3 startT;
    Vector3 endT;
    float dt = 0.0f;
    float start = 0.0f;
    
    auto kf = channel.keyframePairFromTime(Keyframe::kTranslateX, animTime);
    if (!kf.first) return;
    //printf("qx[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
    startT.x = kf.first->v;
    endT.x = kf.second->v;
    float kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;

    kf = channel.keyframePairFromTime(Keyframe::kTranslateY, animTime);
    if (!kf.first) return;
    //printf("qy[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
    startT.y = kf.first->v;
    endT.y = kf.second->v;
    kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;
    
    kf = channel.keyframePairFromTime(Keyframe::kTranslateZ, animTime);
    if (!kf.first) return;
    //printf("qz[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
    startT.z = kf.first->v;
    endT.z = kf.second->v;
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
    
    translate.x = bx::flerp(startT.x, endT.x, factor);
    translate.y = bx::flerp(startT.y, endT.y, factor);
    translate.z = bx::flerp(startT.z, endT.z, factor);
}

void interpScaleFromSequenceChannel
(
    Vector3& scale,
    const SequenceChannel& channel,
    float animTime
)
{
    Vector3 startT;
    Vector3 endT;
    float dt = 0.0f;
    float start = 0.0f;
    
    auto kf = channel.keyframePairFromTime(Keyframe::kScaleX, animTime);
    if (!kf.first) return;
    //printf("qx[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
    startT.x = kf.first->v;
    endT.x = kf.second->v;
    float kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;

    kf = channel.keyframePairFromTime(Keyframe::kScaleY, animTime);
    if (!kf.first) return;
    //printf("qy[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
    startT.y = kf.first->v;
    endT.y = kf.second->v;
    kfDt = kf.second->t - kf.first->t;
    if (kfDt >= ckm::epsilon<float>() && (kfDt < dt || dt < ckm::epsilon<float>()))
        dt = kfDt;
    if (kf.first->t > start)
        start = kf.first->t;
    
    kf = channel.keyframePairFromTime(Keyframe::kScaleZ, animTime);
    if (!kf.first) return;
    //printf("qz[%.4f]: <%.4f,%.4f>\n", animTime, kf.first->v, kf.second->v);
    startT.z = kf.first->v;
    endT.z = kf.second->v;
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
    
    scale.x = bx::flerp(startT.x, endT.x, factor);
    scale.y = bx::flerp(startT.y, endT.y, factor);
    scale.z = bx::flerp(startT.z, endT.z, factor);
}
    
        
    }   // namespace gfx
}   // namespace cinek