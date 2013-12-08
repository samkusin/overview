//
//  SDL/Renderer.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_SDL_Renderer_hpp
#define CK_Graphics_SDL_Renderer_hpp

#include "Graphics/Renderer.hpp"

#include "SDL2/SDL_render.h"

namespace cinekine {
    namespace glx {
    
    //  The top-level Renderer object
    //      The Renderer handles drawing to the display.
    class SDLRenderer: public Renderer
    {
    public:        
        SDLRenderer(const RendererInitParameters& initParams,
                 SDL_Window* window,
                 const Allocator& allocator);
        virtual ~SDLRenderer();
        
        operator bool() const {
            return _renderer != NULL;
        }
        
        ///////////////////////////////////////////////////////////////////////
        // Resource Management
        //
        //  Creates a renderable texture from a file source
        TexturePtr loadTexture(const char* pathname);
        //  Creates a renderable texture from a byte array.  The width and height
        //  and stride are *source* buffer values.  The final texture width, height
        //  and format depend on the Renderer implementation and platform.
        TexturePtr createTextureFromBuffer(uint32_t w, uint32_t h,
                                           cinek_pixel_format format,
                                           const uint8_t* bytes);
        ///////////////////////////////////////////////////////////////////////
        //  Renderer Draw State
        //
        virtual void begin();
        virtual void display();
        virtual Rect getViewport() const;
        virtual void setViewport(const Rect& rect);
        virtual void enableScissor();
        virtual void disableScissor();
        virtual void setScissor(const Rect& rect);

        //  Specialized clear method, which clears the current rendering target
        virtual void clear(const RGBAColor& color);
    
        virtual void drawTextureRect(const Texture& texture,
                                     const Rect& source, const Rect& dest,
                                     const RGBAColor& color);

        virtual void drawMeshVertices(const Texture& texture, Mesh::Type meshType,
                                      const cinekine::vector<glm::vec2>& vertsPos,
                                      const cinekine::vector<glm::vec2>& vertsUV,
                                      const cinekine::vector<glm::vec4>& vertsColor,
                                      const cinekine::vector<uint16_t>& indices);

        virtual MeshPtr createMesh(TexturePtr& texture,
                                   Mesh::Type meshType,
                                   const cinekine::vector<glm::vec2>& vertsPos,
                                   const cinekine::vector<glm::vec2>& vertsUV,
                                   const cinekine::vector<glm::vec4>& vertsColor,
                                   const cinekine::vector<uint16_t>& indices);

        virtual void drawMesh(const Mesh& mesh, const glm::vec2& position);

        ///////////////////////////////////////////////////////////////////////
        //  Renderer Implementation Specific Accessors
        //  Access to SDL system renderer
        SDL_Renderer* getSDLRenderer() const {
            return _renderer;
        }

    private:
        SDL_Renderer* _renderer;
    };
    

    }   // namespace glx
}   // namespace cinekine

#endif
