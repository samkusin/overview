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
 * @file    GL/GLVertexBatch.hpp
 * @author  Samir Sinha
 * @date    11/22/2013
 * @brief   A GL compatible vertex batch processor (2D)
 * @copyright Cinekine
 */

#ifndef CK_Graphics_GL_VertexBatch_hpp
#define CK_Graphics_GL_VertexBatch_hpp

#include "GLUtils.hpp"
#include "cinek/cpp/ckalloc.hpp"
#include <vector>

namespace cinekine {
    namespace glx {
    
    class GLVertexBatch
    {
        CK_CLASS_NON_COPYABLE(GLVertexBatch);
        
    public:
        GLVertexBatch();
        GLVertexBatch(GLVertexBatch&& other);
        GLVertexBatch(GLenum mode, size_t count, const Allocator& allocator);
        ~GLVertexBatch();
        
        GLVertexBatch& operator=(GLVertexBatch&& other);
        
        void draw();

        size_t size() const {
            return _pos.size();
        }
        bool full() const {
            return _pos.size() == _pos.capacity();
        }
        
        void pushPos(float&& x, float&& y);
        void pushUV(float&& u, float&& v);
        void pushColor(const glm::vec4& color);

    private:
        void deleteBuffers();
        GLenum _mode;
        GLuint _vao;
        GLuint _vboPos;
        GLuint _vboUV;
        GLuint _vboColor;
        size_t _primCount;

        std::vector<glm::vec2, std_allocator<glm::vec2>> _pos;
        std::vector<glm::vec2, std_allocator<glm::vec2>> _uv;
        std::vector<glm::vec4, std_allocator<glm::vec4>> _color;
    };

    }   // namespace glx
}   // namespace cinekine

#endif
