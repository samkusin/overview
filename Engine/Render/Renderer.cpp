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
        _bitmapLibrary(*this),
        _fontLibrary(*this, 10 /* TODO make this font limit configurable. */)
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

    unique_ptr<Texture> Renderer::loadTexture(const char* pathname)
    {
        // TODO - perhaps we need a make_unique_ptr (C++11 version) to confirm that the allocator
        //  creating the item is also used to delete the item.
        unique_ptr<Texture> texture(_allocator.newItem<SDLTexture>(*this, pathname), _allocator);
        return std::move(texture);
    }
    
    void Renderer::begin()
    {
        SDL_RenderClear(_renderer);
    }
        
    void Renderer::end()
    {
        SDL_RenderPresent(_renderer);
    }
        
    }   // namespace ovengine
}   // namespace cinekine
