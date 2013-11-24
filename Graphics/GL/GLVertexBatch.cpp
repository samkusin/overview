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
   
    GLVertexBatch::GLVertexBatch() :
        _mode(0),
        _vao(0),
        _vboPos(0),
        _vboUV(0),
        _vboColor(0),
        _primCount(0),
        _pos(std_allocator<glm::vec2>()),
        _uv(std_allocator<glm::vec2>()),
        _color(std_allocator<glm::vec4>())
    {
            
    }

    GLVertexBatch::GLVertexBatch(GLVertexBatch&& other) :
        _pos(std::move(other._pos)),
        _uv(std::move(other._uv)),
        _color(std::move(other._color))
    {
        deleteBuffers();
        _mode = other._mode;
        _vao = other._vao;
        _vboPos = other._vboPos;
        _vboUV = other._vboUV;
        _vboColor = other._vboColor;
        _primCount = other._primCount;
        other._mode = 0;
        other._vao = 0;
        other._vboPos = 0;
        other._vboUV = 0;
        other._vboColor = 0;
        other._primCount = 0;
    }
    
    GLVertexBatch::GLVertexBatch(GLenum mode, size_t count, const Allocator& allocator) :
        _mode(mode),
        _vao(0),
        _vboPos(0),
        _vboUV(0),
        _vboColor(0),
        _primCount(count),
        _pos(std_allocator<glm::vec2>(allocator)),
        _uv(std_allocator<glm::vec2>(allocator)),
        _color(std_allocator<glm::vec4>(allocator))
    {
        size_t limit = 0;
        if (mode == GL_TRIANGLES)
        {
            limit = _primCount * 3;
        }
        else if (mode == GL_POINTS)
        {
            limit = _primCount;
        }
        else if (mode == GL_LINES)
        {
            limit = _primCount * 2;
        }
        else if (mode == GL_LINE_STRIP)
        {
            limit = _primCount+1;
        }
        
        _pos.reserve(limit);
        _uv.reserve(limit);
        _color.reserve(limit);
        
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        
        glGenBuffers(1, &_vboPos);
        glGenBuffers(1, &_vboUV);
        glGenBuffers(1, &_vboColor);
        
        glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
        //glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ARRAY_BUFFER, limit * sizeof(glm::vec2), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrPos);
        glVertexAttribPointer(kGL_ShaderVertexAttrPos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, _vboUV);
        //glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ARRAY_BUFFER, limit * sizeof(glm::vec2), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrUVs);
        glVertexAttribPointer(kGL_ShaderVertexAttrUVs, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
        //glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ARRAY_BUFFER, limit * sizeof(glm::vec4), NULL, GL_STREAM_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrColor);
        glVertexAttribPointer(kGL_ShaderVertexAttrColor, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindVertexArray(0);
    }
    
    GLVertexBatch::~GLVertexBatch()
    {
        deleteBuffers();
    }
    
    GLVertexBatch& GLVertexBatch::operator=(GLVertexBatch&& other)
    {
        deleteBuffers();
        
        _pos = std::move(other._pos);
        _uv = std::move(other._uv);
        _color = std::move(other._color);
        _mode = other._mode;
        _vao = other._vao;
        _vboPos = other._vboPos;
        _vboUV = other._vboUV;
        _vboColor = other._vboColor;
        _primCount = other._primCount;
        other._vao = 0;
        other._vboPos = 0;
        other._vboUV = 0;
        other._vboColor = 0;
        other._primCount = 0;
        
        return *this;
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
        
    void GLVertexBatch::draw()
    {
        if (_pos.size())
        {
            glBindVertexArray(_vao);

            glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
            //glBufferData(GL_ARRAY_BUFFER, _pos.size() * sizeof(glm::vec2), (GLvoid*)_pos.data(), GL_STREAM_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, _pos.size() * sizeof(glm::vec2), (GLvoid*)_pos.data());
            glBindBuffer(GL_ARRAY_BUFFER, _vboUV);
            //glBufferData(GL_ARRAY_BUFFER, _uv.size() * sizeof(glm::vec2), (GLvoid*)_uv.data(), GL_STREAM_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, _uv.size() * sizeof(glm::vec2), (GLvoid*)_uv.data());
            glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
            //glBufferData(GL_ARRAY_BUFFER, _color.size() * sizeof(glm::vec4), (GLvoid*)_color.data(), GL_STREAM_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, _color.size() * sizeof(glm::vec4), (GLvoid*)_color.data());
            
            glDrawArrays(_mode, 0, _pos.size());
       
            glBindVertexArray(0);
        }
        _pos.clear();
        _uv.clear();
        _color.clear();
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
