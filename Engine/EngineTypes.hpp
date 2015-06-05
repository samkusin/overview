///
/// @file
/// Header documentation for an engine component
///
/// @author     Samir Sinha
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Engine_hpp
#define Overview_Engine_hpp

#include "CKGfx/VertexTypes.hpp"
#include <cinek/types.hpp>

namespace cinek { namespace overview {

using Vector3 = gfx::Vector3;
using Vector4 = gfx::Vector4;
using Matrix4 = gfx::Matrix4;

inline Vector3& operator+=(Vector3& l, const Vector3& r)
{
    bx::vec3Add(l, l, r);
    return l;
}

inline Vector3& operator-=(Vector3& l, const Vector3& r)
{
    bx::vec3Sub(l, l, r);
    return l;
}

inline Vector3 operator+(const Vector3& l, const Vector3& r)
{
    Vector3 result;
    bx::vec3Add(result, l, r);
    return result;
}

inline Vector3 operator-(const Vector3& l, const Vector3& r)
{
    Vector3 result;
    bx::vec3Sub(result, l, r);
    return result;
}

inline bool operator==(const Vector3& l, const Vector3& r)
{
    return l.comp[0]==r.comp[0] && l.comp[1]==r.comp[1] && l.comp[2]==r.comp[2];
}

inline bool operator!=(const Vector3& l, const Vector3& r)
{
    return l.comp[0]!=r.comp[0] || l.comp[1]!=r.comp[1] || l.comp[2]!=r.comp[2];
}

inline Vector3 operator/(const Vector3& v, float scalar)
{
    Vector3 result;
    bx::vec3Mul(result, v, 1/scalar);
    return result;
}


inline Vector3 Matrix4TranslateComponent(const Matrix4& mat)
{
    Vector3 res;
    res.comp[0] = mat.comp[12];
    res.comp[1] = mat.comp[13];
    res.comp[2] = mat.comp[13];
    return res;
}

using EntityId = uint64_t;

} /* namespace overview */ } /* namespace cinek */

#endif
