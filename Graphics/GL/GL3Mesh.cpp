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

#include "GL3Mesh.hpp"
#include "GL3Texture.hpp"
#include "GLShaderAttributes.hpp"
#include "Graphics/RenderDebug.hpp"

namespace cinekine {
    namespace glx {

    GL3Mesh::GL3Mesh(TexturePtr& texture,
                Mesh::Type meshType,
                const cinekine::vector<glm::vec2>& vertsPos,
                const cinekine::vector<glm::vec2>& vertsUV,
                const cinekine::vector<glm::vec4>& vertsColor,
                const cinekine::vector<uint16_t>& indices) :
        _texture(texture),
        _indexCount(0),
        _mode(0),
        _vao(0),
        _vboPos(0),
        _vboUV(0),
        _vboColor(0),
        _ibo(0)
    {
        switch (meshType)
        {
        case Mesh::Type::kTriangles:        _mode = GL_TRIANGLES;       break;
        case Mesh::Type::kTriangleFan:      _mode = GL_TRIANGLE_FAN;    break;
        default:
            RENDER_LOG_ERROR("GL3Mesh - unsupported mode %d", _mode);
            return;
        }
        glGenVertexArrays(1, &_vao);
        
        glGenBuffers(1, &_vboPos);
        glGenBuffers(1, &_vboUV);
        glGenBuffers(1, &_vboColor);
        glGenBuffers(1, &_ibo);
        
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vboPos);
        glBufferData(GL_ARRAY_BUFFER, vertsPos.size() * sizeof(glm::vec2), vertsPos.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrPos);
        glVertexAttribPointer(kGL_ShaderVertexAttrPos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, _vboUV);
        glBufferData(GL_ARRAY_BUFFER, vertsUV.size() * sizeof(glm::vec2), vertsUV.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrUVs);
        glVertexAttribPointer(kGL_ShaderVertexAttrUVs, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, _vboColor);
        glBufferData(GL_ARRAY_BUFFER, vertsColor.size() * sizeof(glm::vec4), vertsColor.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(kGL_ShaderVertexAttrColor);
        glVertexAttribPointer(kGL_ShaderVertexAttrColor, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
        
        glBindVertexArray(0);

        _indexCount = indices.size();
    }

    GL3Mesh::~GL3Mesh()
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
        if (_ibo)
        {
            glDeleteBuffers(1, &_ibo);
            _ibo = 0;
        }
    } 
    
    void GL3Mesh::draw() const
    {
        GLuint textureId = static_cast<GL3Texture*>(_texture.get())->textureID();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBindVertexArray(_vao);
        glDrawElements(_mode, _indexCount, GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
    }

    }   // namespace glx
}   // namespace cinekine
