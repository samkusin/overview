//
//  GL/GLUtils.hpp
//  Overview
//
//  Created by Samir Sinha on 11/17/2013.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_GL_Utils_hpp
#define CK_Graphics_GL_Utils_hpp

#include "Graphics/RenderDebug.hpp"
#include "Graphics/RenderTypes.hpp"

#include "cinek/core/cktypes.h"

#ifdef CK_TARGET_OSX
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h> 
#endif

namespace cinekine {
    namespace glx {

    inline bool GLError(const char* context) {
        GLenum glErr = glGetError();
        if (glErr == GL_NO_ERROR)
            return false;
        RENDER_LOG_ERROR( "%s : GL error 0x%x", context, glErr );
        return true;
    }

 //   GLuint loadShader(const char *)

    }   // namespace glx
}   // namespace cinekine

#endif