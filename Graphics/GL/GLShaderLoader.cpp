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
 */

#include "GLShaderLoader.hpp"
#include "Graphics/RenderDebug.hpp"
#include "Stream/FileStreamBuf.hpp"

namespace cinekine {
    namespace glx {

    GLuint GLShaderLoader::load(const char* pathname, GLenum type)
    {
        FileStreamBuf shaderFile(pathname);

        if (!shaderFile)
            return 0;

        GLuint shader = glCreateShader(type);
        if (GLError("GLShaderLoader.glCreateShader"))
            return 0;

        std::istreambuf_iterator<char> itInput(&shaderFile);
        std::istreambuf_iterator<char> itEnd;
        string source(itInput, itEnd);
        const char* sourceCData = source.data();
        glShaderSource(shader, 1, &sourceCData, NULL);
        glCompileShader(shader);

        GLint result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            RENDER_LOG_ERROR("GLShaderLoader.glCompileShader - compile failed");
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);
            if (result)
            {
                //  reserve enough space in the return vector for the shader log.
                _log.resize(result);
                char* logPtr = &_log.front();
                glGetShaderInfoLog(shader, result, NULL, logPtr);
                RENDER_LOG_WARN(logPtr);
            }
            glDeleteShader(shader);
            shader = 0;
        }
        return shader;
    }
    
    }   // namespace glx
}   // namespace cinekine
