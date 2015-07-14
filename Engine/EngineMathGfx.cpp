//
//  EngineMathGfx.cpp
//  Overview
//
//  Created by Samir Sinha on 7/9/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "EngineMathGfx.hpp"
#include "CKGfx/VertexTypes.hpp"

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace ckm {

    template<>
    cinek::gfx::Matrix4& convert<mat4, cinek::gfx::Matrix4>
    (
        mat4 const& src,
        cinek::gfx::Matrix4& dest
    )
    {
        dest[0]     = src[0].x;
        dest[1]     = src[0].y;
        dest[2]     = src[0].z;
        dest[3]     = src[0].w;
        dest[4]     = src[1].x;
        dest[5]     = src[1].y;
        dest[6]     = src[1].z;
        dest[7]     = src[1].w;
        dest[8]     = src[2].x;
        dest[9]     = src[2].y;
        dest[10]    = src[2].z;
        dest[11]    = src[2].w;
        dest[12]    = src[3].x;
        dest[13]    = src[3].y;
        dest[14]    = src[3].z;
        dest[15]    = src[3].w;
        
        return dest;
    }

}