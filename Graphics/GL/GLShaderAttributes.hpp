/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * @file    GL/GLShaderAttributes.hpp
 * @author  Samir Sinha
 * @date    11/19/2013
 * @brief   Fixed locations for shader attributes used by the renderer
 * @copyright Cinekine
 */

#ifndef CK_Graphics_GL_GLShaderAttributes_hpp
#define CK_Graphics_GL_GLShaderAttributes_hpp

namespace cinekine {
    namespace glx {

    /** Vertex Position attribute location */
    const int kGL_ShaderVertexAttrPos = 0;
    /** Vertex UV attribute location */
    const int kGL_ShaderVertexAttrUVs = 1;
    /** Vertex Color attribute location */
    const int kGL_ShaderVertexAttrColor = 2;
    /** Fragment color output location */
    const int kGL_ShaderFragDataColor = 0;

    /** The projection matrix uniform attribute name */
    const char* kGL_ShaderUniformProjectionMat = "projectionMat";
    /** The texture (unit 0) sampler */
    const char* kGL_ShaderUniformTextureSampler0 = "textureSampler0";
    /** The color mode (0=textured,1=alpha mask,2=pass-through) */
    const char* kGL_ShaderUniformColorBlendMode = "colorBlendMode";
    /** Is vertex color enabled */
    const char* kGL_ShaderUniformVertexColorEnabled = "vertexColorOn";

    }   // namespace glx
}   // namespace cinekine


#endif
