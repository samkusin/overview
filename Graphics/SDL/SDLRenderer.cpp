//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "SDLRenderer.hpp"
#include "SDLTexture.hpp"
#include "SDLMesh.hpp"

#include "SDL2/SDL_rect.h"

namespace cinekine {
    namespace glx {

    SDLRenderer::SDLRenderer(const RendererInitParameters& initParams,
                       SDL_Window* window,
                       const Allocator& allocator) :
        Renderer(initParams, allocator),
        _renderer(NULL)
    {
        _renderer = SDL_CreateRenderer(window, -1,
                                       SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC);
    }
    
    SDLRenderer::~SDLRenderer()
    {
        SDL_DestroyRenderer(_renderer);
    }

    TexturePtr SDLRenderer::loadTexture(const char* pathname)
    {
        Allocator& allocator = getAllocator();
        return std::allocate_shared<SDLTexture,
                                    std_allocator<SDLTexture>,
                                    SDLRenderer&,
                                    const char*>
            (
                std_allocator<SDLTexture>(allocator), *this, std::move(pathname)
            );
    }

    TexturePtr SDLRenderer::createTextureFromBuffer(uint32_t w, uint32_t h,
                                                    cinek_pixel_format format,
                                                    const uint8_t* bytes)
    {
        Allocator& allocator = getAllocator();
        return std::allocate_shared<SDLTexture,
                                    std_allocator<SDLTexture>,
                                    SDLRenderer&,
                                    uint32_t, uint32_t,
                                    cinek_pixel_format,
                                    const uint8_t*>
            (
                std_allocator<SDLTexture>(allocator), *this,
                                          std::move(w), std::move(h),
                                          std::move(format), std::move(bytes)
            );
    }
    
    void SDLRenderer::begin()
    {
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    }
        
    void SDLRenderer::display()
    {
        SDL_RenderPresent(_renderer);
    }

    Rect SDLRenderer::getViewport() const
    {
        SDL_Rect viewportRect;
        SDL_RenderGetViewport(_renderer, &viewportRect);
        return Rect(viewportRect.x, viewportRect.y,
                    viewportRect.x + viewportRect.w-1, viewportRect.y + viewportRect.h-1);
    }

    void SDLRenderer::setViewport(const Rect& rect)
    {
        SDL_Rect viewportRect;
        viewportRect.x = rect.left;
        viewportRect.y = rect.top;
        viewportRect.w = rect.width();
        viewportRect.h = rect.height();
        SDL_RenderSetViewport(_renderer, &viewportRect);
    }

    void SDLRenderer::enableScissor()
    {
        //  handled by setScissorRect
    }

    void SDLRenderer::disableScissor()
    {
        SDL_RenderSetClipRect(_renderer, NULL);
    }

    void SDLRenderer::setScissor(const Rect& rect)
    {
        SDL_Rect sdlRect;
        sdlRect.x = rect.left;
        sdlRect.y = rect.top;
        sdlRect.w = rect.width();
        sdlRect.h = rect.height();
        SDL_RenderSetClipRect(_renderer, &sdlRect);
    }

    void SDLRenderer::clear(const RGBAColor& color)
    {
        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(_renderer);
    }

    void SDLRenderer::drawTextureRect(const Texture& texture,
                                      const Rect& source, const Rect& dest,
                                      const RGBAColor& color)
    {
        SDL_Texture* sdlTexture = ((const SDLTexture& )texture).getSDLTexture();
        if (sdlTexture)
        {
            SDL_Rect srcRect;
            SDL_Rect destRect;
            srcRect.x = source.left;
            srcRect.y = source.top;
            srcRect.w = source.width();
            srcRect.h = source.height();
            destRect.x = dest.left;
            destRect.y = dest.top;
            destRect.w = dest.width();
            destRect.h = dest.height();
            SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureColorMod(sdlTexture, color.r, color.g, color.b);
            SDL_SetTextureAlphaMod(sdlTexture, color.a);
            SDL_RenderCopy(_renderer, sdlTexture, &srcRect, &destRect);
        }      
    }


    void SDLRenderer::drawMeshVertices(const Texture& texture, Mesh::Type meshType,
                                       const cinekine::vector<glm::vec2>& vertsPos,
                                       const cinekine::vector<glm::vec2>& vertsUV,
                                       const cinekine::vector<glm::vec4>& vertsColor,
                                       const cinekine::vector<uint16_t>& indices)
    {
        /**
         * @todo SDL's accelerated renderer does not offer a uniform way to render
         * a 2D mesh.  We must fall back to the underlying renderer.
         *
         * Windows and Linux support OpenGL 2.1 so that will likely be the reference
         * renderer.
         */
    }

    MeshPtr SDLRenderer::createMesh(TexturePtr& texture,
                                    Mesh::Type meshType,
                                    const cinekine::vector<glm::vec2>& vertsPos,
                                    const cinekine::vector<glm::vec2>& vertsUV,
                                    const cinekine::vector<glm::vec4>& vertsColor,
                                    const cinekine::vector<uint16_t>& indices)
    {
        Allocator& allocator = getAllocator();
        return std::allocate_shared<SDLMesh,
                                    std_allocator<SDLMesh>>(std_allocator<SDLMesh>(allocator));
    }

    void SDLRenderer::drawMesh(const Mesh& mesh, const glm::vec2& position)
    {
        /**
         * @todo See drawMeshVertices
         */
    }

    }   // namespace glx
}   // namespace cinekine
