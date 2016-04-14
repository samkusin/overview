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

#include <bx/fpumath.h>

namespace ckm {

template struct vector2_type<scalar>;
template struct vector3_type<scalar>;
template struct vector4_type<scalar>;
template struct matrix4_type<scalar>;
template struct quat_type<scalar>;


template<> float abs<float>(float v) {
    return std::fabs(v);
}
template<> float cos<float>(float r) {
    return std::cosf(r);
}
template<> float acos(float a) {
    return std::acosf(a);
}
template<> float sin<float>(float r) {
    return std::sinf(r);
}
template<> float asin<float>(float a) {
    return std::asinf(a);
}
template<> float tan<float>(float r) {
    return std::tanf(r);
}
template<> float atan<float>(float a) {
    return std::atanf(a);
}
template<> float radians<float>(float degrees) {
    return kPi * degrees / 180.0f;
}
template<> float degrees<float>(float radians) {
    return 180.0f * radians / kPi;
}

//  operations
template<> vector4& add
(
    vector4& r,
    vector4 const& a,
    vector4 const& b
)
{
    r.comp[0] = a.comp[0] + b.comp[0];
    r.comp[1] = a.comp[1] + b.comp[1];
    r.comp[2] = a.comp[2] + b.comp[2];
    r.comp[3] = a.comp[3] + b.comp[3];
    
    return r;
}

template<> vector3_type<float>& add
(
    vector3_type<float>& r,
    vector3_type<float> const& a,
    vector3_type<float> const& b
)
{
    bx::vec3Add(r.comp, a.comp, b.comp);
    return r;
}

template<> vector2& add
(
    vector2& r,
    vector2 const& a,
    vector2 const& b
)
{
    r.comp[0] = a.comp[0] + b.comp[0];
    r.comp[1] = a.comp[1] + b.comp[1];
    return r;
}

template<> vector4& sub
(
    vector4& r,
    vector4 const& a,
    vector4 const& b
)
{
    r.comp[0] = a.comp[0] - b.comp[0];
    r.comp[1] = a.comp[1] - b.comp[1];
    r.comp[2] = a.comp[2] - b.comp[2];
    r.comp[3] = a.comp[3] - b.comp[3];
    
    return r;
}

template<> vector3_type<float>& sub
(
    vector3_type<float>& r,
    vector3_type<float> const& a,
    vector3_type<float> const& b
)
{
    bx::vec3Sub(r.comp, a.comp, b.comp);
    return r;
}

template<> vector2& sub
(
    vector2& r,
    vector2 const& a,
    vector2 const& b
)
{
    r.comp[0] = a.comp[0] - b.comp[0];
    r.comp[1] = a.comp[1] - b.comp[1];
    return r;
}

template<> vector3_type<float>& scale
(
    vector3_type<float>& r,
    vector3_type<float> const& v,
    typename vector3_type<float>::value_type s
)
{
    bx::vec3Mul(r.comp, v.comp, s);
    return r;
}

template<> vector2& scale
(
    vector2& r,
    vector2 const& v,
    typename vector2::value_type s
)
{
    r.comp[0] = v.comp[0] * s;
    r.comp[1] = v.comp[1] * s;
    return r;
}

template<> vector3_type<float>& cross
(
    vector3_type<float>& r,
    vector3_type<float> const& x,
    vector3_type<float> const& y
)
{
    bx::vec3Cross(r.comp, x.comp, y.comp);
    return r;
}

template<> typename vector3_type<float>::value_type dot
(
    vector3_type<float> const& v0,
    vector3_type<float> const& v1
)
{
    return bx::vec3Dot(v0.comp, v1.comp);
}

template<> typename vector3_type<float>::value_type vectorLength(vector3_type<float> const& v)
{
    return bx::vec3Length(v.comp);
}

template<> typename vector2_type<float>::value_type vectorLength(vector2_type<float> const& v)
{
    return sqrtf(v.x*v.x + v.y*v.y);
}

//  val_type can either be vector or matrix
template<> matrix4_type<float>& inverse(matrix4_type<float>& o, matrix4_type<float> const& m)
{
    bx::mtxInverse(o.comp, m.comp);
    return o;
}

template<> vector3_type<float>& normalize(vector3_type<float>& o, vector3_type<float> const& v)
{
    bx::vec3Norm(o.comp, v.comp);
    return o;
}

template<> vector2& normalize(vector2& o, vector2 const& v)
{
    auto idist = 1.0f/vectorLength(v);
    o[0] = v[0] * idist;
    o[1] = v[1] * idist;
    return o;
}

template<> vector4& mul(vector4_type<float>& r, vector4_type<float> const& v, matrix4_type<float> const& v2)
{
    bx::vec4MulMtx(r.comp, v.comp, v2.comp);
    return r;
}

template<> vector3_type<float>& mul(vector3_type<float>& r, vector3_type<float> const& v, matrix4_type<float> const& v2)
{
    bx::vec3MulMtx(r.comp, v.comp, v2.comp);
    return r;
}

template<> matrix4_type<float>& mul(matrix4_type<float>& r, matrix4_type<float> const& m0, matrix4_type<float> const& m1)
{
    bx::mtxMul(r.comp, m0.comp, m1.comp);
    return r;
}

template<> matrix4_type<float>& eulerToMatrix(matrix4_type<float>& m, float ax, float ay, float az)
{
    bx::mtxRotateXYZ(m.comp, ax, ay, az);
    return m;
}



template struct plane<vector3>;
template class frustrum<vector3>;
template struct AABB<vector3>;
template struct raytest<scalar>;

}
