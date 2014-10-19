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
 * @file    GL/GL3Mesh.hpp
 * @author  Samir Sinha
 * @date    11/29/2013
 * @brief   GL3 specific mesh objects (VBO GL 3.2 based)
 * @copyright Cinekine
 */

#ifndef CK_Graphics_GL3_Mesh_hpp
#define CK_Graphics_GL3_Mesh_hpp

#include "GLUtils.hpp"

#include "Graphics/Mesh.hpp"
#include "Graphics/Texture.hpp"

#include "cinek/vector.hpp"

namespace cinek {
    namespace glx {

    /**
     * @class GL3Mesh
     * @brief GL3 (OpenGL3.x VBO based) specific mesh objects
     */
    class GL3Mesh: public Mesh
    {
    public:
        GL3Mesh(TexturePtr& texture,
                Mesh::Type meshType,
                const cinek::vector<glm::vec2>& vertsPos,
                const cinek::vector<glm::vec2>& vertsUV,
                const cinek::vector<glm::vec4>& vertsColor,
                const cinek::vector<uint16_t>& indices);

        virtual ~GL3Mesh();
        /** @return Checks whether a Mesh was created successfully */
        virtual operator bool() const {
            return _indexCount != 0;
        }
        void draw() const;

    private:
        TexturePtr _texture;
        size_t _indexCount;
        GLenum _mode;
        GLuint _vao;
        GLuint _vboPos;
        GLuint _vboUV;
        GLuint _vboColor;
        GLuint _ibo;
    };

    }   // namespace glx
}   // namespace cinek

#endif