///
/// @file
/// Header documentation for an engine component
///
/// @author     Samir Sinha
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Engine_Math_Types_hpp
#define Overview_Engine_Math_Types_hpp

#include <cinek/types.hpp>

#define GLM_FORCE_NO_CTOR_INIT
#include <glm/fwd.hpp>

namespace ckm
{
    using scalar = glm::float64;
    using ivec2 = glm::tvec2<int32_t, glm::highp>;
    using vec2 = glm::tvec2<scalar, glm::highp>;
    using vec3 = glm::tvec3<scalar, glm::highp>;
    using vec4 = glm::tvec4<scalar, glm::highp>;
    using mat3 = glm::tmat3x3<scalar, glm::highp>;
    using mat4 = glm::tmat4x4<scalar, glm::highp>;
    using quat = glm::tquat<scalar, glm::highp>;
    
    constexpr ckm::scalar kHalfScalar = 0.5;
    
    template<typename _Point> struct AABB;
    template<typename _Point> struct AABB2;
    
    struct Plane3;
    class Frustrum;
}

#endif
