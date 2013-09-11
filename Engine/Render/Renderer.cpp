//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Renderer.hpp"
#include "SDL/SDLTexture.hpp"

#include "Engine/Defines.hpp"
#include "Engine/Theater.hpp"
#include "Engine/SpriteDatabaseLoader.hpp"
#include "Engine/TileDatabaseLoader.hpp"


namespace cinekine {
    namespace ovengine {

    Renderer::Renderer(Theater& theater, SDL_Window* window, const InitParameters& initParams,
                       const Allocator& allocator) :
        _allocator(allocator),
        _theater(theater),
        _renderer(NULL),
        _bitmapLibrary(*this, initParams.bitmapAtlasDir)
    {
        _renderer = SDL_CreateRenderer(window, -1,
                                       SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC);
        //  Setup Theater <-> Renderer interaction (Director <-> View by proxy)
        //
        //  handle sprite -> bitmap loading.
        theater.getSpriteDatabaseLoader().onBitmapAtlasRequest([this]
            (const char* atlasName) -> cinek_bitmap_atlas
            {
                return this->getBitmapLibrary().loadAtlas(atlasName);
            }
        );
        
        theater.getSpriteDatabaseLoader().onBitmapIndexRequest([this]
            ( cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
            {
                 const ovengine::BitmapAtlas* bitmapAtlas = this->getBitmapLibrary().getAtlas(atlas);
                 if (!bitmapAtlas)
                     return kCinekBitmapIndex_Invalid;
                 return bitmapAtlas->getBitmapIndex(name);
            }
        );
        //  handle tile -> bitmap loading.
        theater.getTileDatabaseLoader().onBitmapAtlasRequest([this]
            (const char* atlasName) -> cinek_bitmap_atlas
            {
                return this->getBitmapLibrary().loadAtlas(atlasName);
            }
        );
        theater.getTileDatabaseLoader().onBitmapIndexRequest([this]
            (cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
            {
               const ovengine::BitmapAtlas* bitmapAtlas = this->getBitmapLibrary().getAtlas(atlas);
               if (!bitmapAtlas)
                   return kCinekBitmapIndex_Invalid;
               return bitmapAtlas->getBitmapIndex(name);
            }
        );
    }
    
    Renderer::~Renderer()
    {
        SDL_DestroyRenderer(_renderer);
    }
    
    void Renderer::begin()
    {
        SDL_RenderClear(_renderer);
    }
        
    void Renderer::end()
    {
        SDL_RenderPresent(_renderer);
    }
    
    SDL_Rect Renderer::getViewport() const
    {
        SDL_Rect viewportRect;
        SDL_RenderGetViewport(_renderer, &viewportRect);
        return viewportRect;
    }
    
    unique_ptr<Texture> Renderer::loadTexture(const char* pathname)
    {
        // TODO - perhaps we need a make_unique_ptr (C++11 version) to confirm that the allocator
        //  creating the item is also used to delete the item.
        unique_ptr<Texture> texture(_allocator.newItem<SDLTexture>(*this, pathname), _allocator);
        return std::move(texture);
    }
    
    void Renderer::drawBitmap(const cinek_bitmap &bitmap, int32_t x, int32_t y)
    {
        if (bitmap.bmpClass != _currentAtlasIndex)
        {
            _currentAtlasIndex = bitmap.bmpClass;
            _currentAtlas = _bitmapLibrary.getAtlas(_currentAtlasIndex);
        }
        if (_currentAtlas)
        {
            const SDLTexture& texture = (SDLTexture&)_currentAtlas->getTexture();
            SDL_Texture* sdlTexture = texture.getSDLTexture();
            const ovengine::BitmapInfo* bitmapInfo = _currentAtlas->getBitmapFromIndex(bitmap.bmpIndex);
            if (bitmapInfo)
            {
                SDL_Rect srcRect;
                SDL_Rect destRect;
                srcRect.x = bitmapInfo->x;
                srcRect.y = bitmapInfo->y;
                srcRect.w = bitmapInfo->w;
                srcRect.h = bitmapInfo->h;
                destRect.x = x + bitmapInfo->offX;
                destRect.y = y - bitmapInfo->srcH + bitmapInfo->offY;
                destRect.w = bitmapInfo->w;
                destRect.h = bitmapInfo->h;
                SDL_RenderCopy(_renderer, sdlTexture, &srcRect, &destRect);
            }
         }
    }
        
    }   // namespace ovengine
}   // namespace cinekine
