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
#include "cinek/cpp/ckvector.hpp"
 
namespace cinekine {
    namespace glx {
    
    class GLVertexBatch
    {
        CK_CLASS_NON_COPYABLE(GLVertexBatch);
        
    public:
        GLVertexBatch(GLenum usage,
                      vector<glm::vec2>& pos,
                      vector<glm::vec2>& uv,
                      vector<glm::vec4>& color);
        GLVertexBatch(GLVertexBatch&& other);
        ~GLVertexBatch();
        
        //void reset();
        void draw(GLenum mode);

        size_t size() const {
            return _pos.size();
        }
        bool empty() const {
            return _pos.empty();
        }
        bool full() const {
            return _pos.size() == _pos.capacity();
        }
        size_t available() const {
            return _pos.capacity() - _pos.size();
        }
        
        void pushPos(float x, float y) {
            _pos.emplace_back(x, y);
        }
        void pushPos(const vector<glm::vec2>& posv) {
            _pos.insert(_pos.end(), posv.begin(), posv.end());
        }
        void pushUV(float u, float v) {
            _uv.emplace_back(u, v);
        }
        void pushUV(const vector<glm::vec2>& uvv) {
            _uv.insert(_uv.end(), uvv.begin(), uvv.end());
        }
        void pushColor(const glm::vec4& color) {
            _color.emplace_back(color);
        }
        void pushColor(const vector<glm::vec4>& color) {
            _color.insert(_color.end(), color.begin(), color.end());
        }

    private:
        void deleteBuffers();
        void bindVertices();
        GLenum _usage;
        GLuint _vao;
        GLuint _vboPos;
        GLuint _vboUV;
        GLuint _vboColor;
        size_t _capacity;

        vector<glm::vec2>& _pos;
        vector<glm::vec2>& _uv;
        vector<glm::vec4>& _color;
    };

    }   // namespace glx
}   // namespace cinekine

#endif
