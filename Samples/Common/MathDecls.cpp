//
//  MathDecls.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 2/15/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include <ckm/math.hpp>
#include <ckm/geometry.hpp>
#include <ckm/aabb.hpp>

#include <ckm/mathtypes.inl>
#include <ckm/math.inl>
#include <ckm/geometry.inl>

#include <bx/fpumath.h>

namespace ckm {

template struct vector2<float>;
template struct vector3<float>;
template struct vector4<float>;
template struct matrix4<float>;


//  operations
template<> vector4f& add
(
    vector4f& r,
    vector4f const& a,
    vector4f const& b
)
{
    r.comp[0] = a.comp[0] + b.comp[0];
    r.comp[1] = a.comp[1] + b.comp[1];
    r.comp[2] = a.comp[2] + b.comp[2];
    r.comp[3] = a.comp[3] + b.comp[3];
    
    return r;
}

template<> vector3f& add
(
    vector3f& r,
    vector3f const& a,
    vector3f const& b
)
{
    bx::vec3Add(r.comp, a.comp, b.comp);
    return r;
}

template<> vector2f& add
(
    vector2f& r,
    vector2f const& a,
    vector2f const& b
)
{
    r.comp[0] = a.comp[0] + b.comp[0];
    r.comp[1] = a.comp[1] + b.comp[1];
    return r;
}

template<> vector4f& sub
(
    vector4f& r,
    vector4f const& a,
    vector4f const& b
)
{
    r.comp[0] = a.comp[0] - b.comp[0];
    r.comp[1] = a.comp[1] - b.comp[1];
    r.comp[2] = a.comp[2] - b.comp[2];
    r.comp[3] = a.comp[3] - b.comp[3];
    
    return r;
}

template<> vector3f& sub
(
    vector3f& r,
    vector3f const& a,
    vector3f const& b
)
{
    bx::vec3Sub(r.comp, a.comp, b.comp);
    return r;
}

template<> vector2f& sub
(
    vector2f& r,
    vector2f const& a,
    vector2f const& b
)
{
    r.comp[0] = a.comp[0] - b.comp[0];
    r.comp[1] = a.comp[1] - b.comp[1];
    return r;
}

template<> vector3f& scale
(
    vector3f& r,
    vector3f const& v,
    typename vector3f::value_type s
)
{
    bx::vec3Mul(r.comp, v.comp, s);
    return r;
}

template<> vector2f& scale
(
    vector2f& r,
    vector2f const& v,
    typename vector2f::value_type s
)
{
    r.comp[0] = v.comp[0] * s;
    r.comp[1] = v.comp[1] * s;
    return r;
}

template<> vector3f& cross
(
    vector3f& r,
    vector3f const& x,
    vector3f const& y
)
{
    bx::vec3Cross(r.comp, x.comp, y.comp);
    return r;
}

template<> typename vector3f::value_type dot
(
    vector3f const& v0,
    vector3f const& v1
)
{
    return bx::vec3Dot(v0.comp, v1.comp);
}

template<> typename vector3f::value_type vectorLength(vector3f const& v)
{
    return bx::vec3Length(v.comp);
}

template<> typename vector2f::value_type vectorLength(vector2f const& v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

//  val_type can either be vector or matrix
template<> matrix4f& inverse(matrix4f& o, matrix4f const& m)
{
    bx::mtxInverse(o.comp, m.comp);
    return o;
}

template<> vector3f& normalize(vector3f& o, vector3f const& v)
{
    bx::vec3Norm(o.comp, v.comp);
    return o;
}

template<> vector2f& normalize(vector2f& o, vector2f const& v)
{
    auto idist = 1.0f/vectorLength(v);
    o[0] = v[0] * idist;
    o[1] = v[1] * idist;
    return o;
}

template<> vector4f& mul(vector4f& r, vector4f const& v, matrix4f const& v2)
{
    bx::vec4MulMtx(r.comp, v.comp, v2.comp);
    return r;
}

template struct Plane3<vector3f>;
template class Frustrum<vector3f>;
template struct AABB<vector3f>;

}
