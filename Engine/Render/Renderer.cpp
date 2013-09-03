//
//  Renderer.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Renderer.hpp"
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
        
    }   // namespace ovengine
}   // namespace cinekine
