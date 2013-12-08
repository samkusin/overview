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
 * @file    RendererCLI.hpp
 * @author  Samir Sinha
 * @date    11/26/2013
 * @brief   The Renderer Client Interface exposed to applications
 * @copyright Cinekine
 */

#ifndef CK_Graphics_RendererCLI_hpp
#define CK_Graphics_RendererCLI_hpp

#include "RenderTypes.hpp"
#include "Rect.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

#include "cinek/cpp/ckvector.hpp"

namespace cinekine {
    namespace glx {
   
    /**
     * @class RendererCLI
     * @brief The Renderer client interface exposed to applications
     */
    class RendererCLI
    {
    public:    
        virtual ~RendererCLI() = default;

        /**
         * Creates a renderable texture from a file source.  The caller is
         * responsible for holding and releasing the texture.
         * 
         * @param  pathname Pathname to the texture file.
         * @return A managed pointer to the texture. 
         */
        virtual TexturePtr loadTexture(const char* pathname) = 0;
        /**
         * Creates a renderable texture from a byte array.  The width and height
         * and stride are *source* buffer values.  The final texture width, height
         * and format depend on the Renderer implementation and platform.    The
         * caller owns the texture.
         *
         * @param  w      Texture width
         * @param  h      Texture height
         * @param  format The pixel format for the texture
         * @param  bytes  A buffer containing the texture's pixels.  If NULL, then
         *                no data is uploaded to the texture
         * @return A managed pointer to the texture.
         */  
        virtual TexturePtr createTextureFromBuffer(uint32_t w, uint32_t h,
                                                   cinek_pixel_format format,
                                                   const uint8_t* bytes) = 0;
        /**
         * @return Returns the current viewport rect
         */
        virtual Rect getViewport() const = 0;
        /**
         * Sets the current rendering viewport
         * @param rect Rectangle using client coordinates (i.e. the window's client
         *             area)
         */
        virtual void setViewport(const Rect& rect) = 0;
        /**
         * Enables scissor testing for supporting renderers
         */
        virtual void enableScissor() = 0;
        /**
         * Disables scissor testing
         */
        virtual void disableScissor() = 0;
        /**
         * Sets the scissor region for supporting renderers
         * @param rect Scissor rectangle
         */
        virtual void setScissor(const Rect& rect) = 0;
        /**
         * Draws a texture to the current rendering target
         * 
         * @param texture Texture object to render
         * @param source  The source rectangle within the texture to render
         * @param dest    The destination rectangle on the rendering target.  
         *                Callers define larger/smaller rectangles to scale or 
         *                shrink accordingly
         * @param color   RGBA values to render, for color/alpha blending
         */
        virtual void drawTextureRect(const Texture& texture,
                                     const Rect& source, const Rect& dest,
                                     const RGBAColor& color) = 0;

        /**
         * Draws a 2D mesh to the current rendering target from the source
         * texture.  All vertex vectors passed to drawMesh must be of the
         * same size.
         * 
         * @param texture    Texture to render
         * @param meshType   How to render the vertices (i.e. triangles,lines)
         * @param vertsPos   Position vertices to render.  
         * @param vertsUV    UV vertices to render
         * @param vertsColor Color vertices to render
         * @param indices    Vertex indicies into the specified arrays
         */
        virtual void drawMeshVertices(const Texture& texture, Mesh::Type meshType,
                                      const cinekine::vector<glm::vec2>& vertsPos,
                                      const cinekine::vector<glm::vec2>& vertsUV,
                                      const cinekine::vector<glm::vec4>& vertsColor,
                                      const cinekine::vector<uint16_t>& indices) = 0;
        /**
         * Creates a 2D Mesh given its texture and vertices.
         *
         * @param texture    Texture to render - this mesh will hold a shared reference
         *                   to the supplied texture
         * @param meshType   How to render the vertices (i.e. triangles,lines)
         * @param vertsPos   Position vertices to render.  
         * @param vertsUV    UV vertices to render
         * @param vertsColor Color vertices to render
         * @param indices    Vertex indicies into the specified arrays
         * @returns A managed pointer to the Mesh object
         */
        virtual MeshPtr createMesh(TexturePtr& texture,
                                   Mesh::Type meshType,
                                   const cinekine::vector<glm::vec2>& vertsPos,
                                   const cinekine::vector<glm::vec2>& vertsUV,
                                   const cinekine::vector<glm::vec4>& vertsColor,
                                   const cinekine::vector<uint16_t>& indices) = 0;
        /**
         * Renders a 2D mesh
         * 
         * @param mesh     The Mesh object to render
         * @param position Screen position
         */
        virtual void drawMesh(const Mesh& mesh, const glm::vec2& position) = 0;
    };

    }   // namespace glx
}   // namespace cinekine

#endif
