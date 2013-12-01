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

#include "GLVertexBatch.hpp"
#include "GLShaderAttributes.hpp"

namespace cinekine {
    namespace glx {

    GLVertexBatch::GLVertexBatch(GLVertexBatch&& other) :
        _pos(other._pos),
        _uv(other._uv),
        _color(other._color)
    {
        deleteBuffers();
        _mode = other._mode;
        _vao = other._vao;
        _vboPos = other._vboPos;
        _vboUV = other._vboUV;
        _vboColor = other._vboColor;
        other._mode = 0;
        other._vao = 0;
        other._vboPos = 0;
        other._vboUV = 0;
        other._vboColor = 0;
    }
    
    GLVertexBatch::GLVertexBatch(GLenum mode,
                      vector<glm::vec2>& pos,
                      vector<glm::vec2>& uv,
                      vector<glm::vec4>& color) :
        _mode(mode),
        _vao(0),
        _vboPos(0),
        _vboUV(0),
        _vboColor(0),
        _pos(pos),
        _uv(uv),
        _color(color)
    {
        glGenVertexArrays(1, &_vao);
        
        glGenBuffers(1, &_vboPos);
        glGenBuffers(1, &_vboUV);
        glGenBuffers(1, &_vboColor);
        
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
        //glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ARRAY_BUFFER, _pos.capacity() * sizeof(glm::vec2), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrPos);
        glVertexAttribPointer(kGL_ShaderVertexAttrPos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, _vboUV);
        //glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ARRAY_BUFFER, _uv.capacity() * sizeof(glm::vec2), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrUVs);
        glVertexAttribPointer(kGL_ShaderVertexAttrUVs, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
        //glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ARRAY_BUFFER, _color.capacity() * sizeof(glm::vec4), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrColor);
        glVertexAttribPointer(kGL_ShaderVertexAttrColor, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindVertexArray(0);
    }
    
    GLVertexBatch::~GLVertexBatch()
    {
        deleteBuffers();
    }
    
    void GLVertexBatch::deleteBuffers()
    {
        if (_vao)
        {
            glDeleteVertexArrays(1, &_vao);
            _vao = 0;
        }
        if (_vboPos)
        {
            glDeleteBuffers(1, &_vboPos);
            _vboPos = 0;
        }
        if (_vboUV)
        {
            glDeleteBuffers(1, &_vboUV);
            _vboUV = 0;
        }
        if (_vboColor)
        {
            glDeleteBuffers(1, &_vboColor);
            _vboColor = 0;
        }
    }

    void GLVertexBatch::reset()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
        glBufferData(GL_ARRAY_BUFFER, _pos.capacity() * sizeof(glm::vec2), NULL, GL_STREAM_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, _vboUV);
        glBufferData(GL_ARRAY_BUFFER, _uv.capacity() * sizeof(glm::vec2), NULL, GL_STREAM_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
        glBufferData(GL_ARRAY_BUFFER, _color.capacity() * sizeof(glm::vec4), NULL, GL_STREAM_DRAW);
    }

    void GLVertexBatch::bindVertices()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
        //glBufferData(GL_ARRAY_BUFFER, _pos.size() * sizeof(glm::vec2), (GLvoid*)_pos.data(), GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _pos.size() * sizeof(glm::vec2), (GLvoid*)_pos.data());
        glBindBuffer(GL_ARRAY_BUFFER, _vboUV);
        //glBufferData(GL_ARRAY_BUFFER, _uv.size() * sizeof(glm::vec2), (GLvoid*)_uv.data(), GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _uv.size() * sizeof(glm::vec2), (GLvoid*)_uv.data());
        glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
        //glBufferData(GL_ARRAY_BUFFER, _color.size() * sizeof(glm::vec4), (GLvoid*)_color.data(), GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _color.size() * sizeof(glm::vec4), (GLvoid*)_color.data());
    }
        
    void GLVertexBatch::draw()
    {
        if (_pos.size())
        {
            glBindVertexArray(_vao);

            bindVertices();
            
            glDrawArrays(_mode, 0, _pos.size());
       
            glBindVertexArray(0);
        }
    }

    void GLVertexBatch::pushPos(float&& x, float&& y)
    {
        _pos.emplace_back(x, y);
    }
    
    void GLVertexBatch::pushUV(float&& u, float&& v)
    {
        _uv.emplace_back(u, v);
    }
    
    void GLVertexBatch::pushColor(const glm::vec4& color)
    {
        _color.emplace_back(color);
    }

    }   // namespace glx
}   // namespace cinekine
