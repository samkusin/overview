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

template struct Plane3<vector3f>;
template class Frustrum<vector3f>;
template struct AABB<vector3f>;

}
