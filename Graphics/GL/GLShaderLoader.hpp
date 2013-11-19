/**
 * @file    GL/GLShaderLoader.hpp
 * @author  Samir Sinha
 * @date    11/18/2013
 * @brief   Utility class to load GL style shaders
 * @copyright Cinekine
 */

/*
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

#include "GLUtils.hpp"

#include "cinek/cpp/ckstring.hpp"

#include <vector>

namespace cinekine {
    namespace glx {

    /**
     * @class GLShaderLoader
     * @brief Loads and builds GLSL compliant shaders
     */
    class GLShaderLoader
    {
    public:
        GLShaderLoader() = default;
        ~GLShaderLoader() = default;

        /**
         * Loads a shader from the specified source file
         * @param  pathname Pathname of the source file
         * @param  type     GL shader type (GL_VERTEX_SHADER, etc)
         * @return          GL shader resource
         */
        GLuint load(const char* pathname, GLenum type);

    private:
        std::vector<char, std_allocator<char>> _log;
    };

    }   // namespace glx
}   // namespace cinekine
