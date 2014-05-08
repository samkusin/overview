//
//  Theater.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Theater.hpp"
#include "TheaterCLI.hpp"
#include "SpriteDatabaseLoader.hpp"
#include "TileDatabaseLoader.hpp"
#include "Debug.hpp"

#include "Framework/FileStreamBuf.hpp"
#include "Graphics/BitmapLibrary.hpp"

#include "cinek/rendermodel/spritedatabase.hpp"
#include "cinek/rendermodel/tiledatabase.hpp"

namespace cinekine {
    namespace ovengine {

    class Theater::Impl: public TheaterCLI
    {
        friend class Theater;

    public:
        Impl(Allocator& allocator);
        ~Impl();

        virtual void loadSpriteDatabase(const char* spriteDbName, glx::BitmapLibrary& bitmapLibrary);
        virtual void loadTileDatabase(const char* tileDbName, glx::BitmapLibrary& bitmapLibrary);
        const rendermodel::TileDatabase& tileDatabase() const;
        const rendermodel::SpriteDatabase& spriteDatabase() const;

    private:
        rendermodel::TileDatabase _tileDb;
        TileDatabaseLoader _tileDbLoader;
        rendermodel::SpriteDatabase _spriteDb;
        SpriteDatabaseLoader _spriteDbLoader;
    };

    ////////////////////////////////////////////////////////////////////////////

    Theater::Impl::Impl(Allocator& allocator) :
        _tileDb(1024, allocator),
        _tileDbLoader(_tileDb),
        _spriteDb(1024, allocator),
        _spriteDbLoader(_spriteDb)
    {
    }

    Theater::Impl::~Impl()
    {
    }

    void Theater::Impl::loadSpriteDatabase(const char* spriteDbName,
                                           glx::BitmapLibrary& bitmapLibrary)
    {
        FileStreamBuf dbStream(spriteDbName);
        if (!dbStream)
            return;

        _spriteDbLoader.onBitmapAtlasRequest([&bitmapLibrary]
            (const char* atlasName) -> cinek_bitmap_atlas
            {
                char path[MAX_PATH];
                snprintf(path, sizeof(path), "textures/%s", atlasName);
                return bitmapLibrary.loadAtlas(path);
            }
        );
        _spriteDbLoader.onBitmapIndexRequest([&bitmapLibrary]
            ( cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
            {
                 const glx::BitmapAtlas* bitmapAtlas = bitmapLibrary.getAtlas(atlas).get();
                 if (!bitmapAtlas)
                     return kCinekBitmapIndex_Invalid;
                 return bitmapAtlas->getBitmapIndex(name);
            }
        );

        _spriteDbLoader.unserialize(dbStream);
    }

    void Theater::Impl::loadTileDatabase(const char* tileDbName,
                                         glx::BitmapLibrary& bitmapLibrary)
    {
        FileStreamBuf dbStream(tileDbName);
        if (!dbStream)
            return;

        _tileDbLoader.onBitmapAtlasRequest([&bitmapLibrary]
            (const char* atlasName) -> cinek_bitmap_atlas
            {
                char path[MAX_PATH];
                snprintf(path, sizeof(path), "textures/%s", atlasName);
                return bitmapLibrary.loadAtlas(path);
            }
        );
        _tileDbLoader.onBitmapIndexRequest([&bitmapLibrary]
            (cinek_bitmap_atlas atlas, const char* name) -> cinek_bitmap_index
            {
               const glx::BitmapAtlas* bitmapAtlas = bitmapLibrary.getAtlas(atlas).get();
               if (!bitmapAtlas)
                   return kCinekBitmapIndex_Invalid;
               return bitmapAtlas->getBitmapIndex(name);
            }
        );

        _tileDbLoader.unserialize(dbStream);
    }

    const rendermodel::TileDatabase& Theater::Impl::tileDatabase() const
    {
        return _tileDb;
    }

    const rendermodel::SpriteDatabase& Theater::Impl::spriteDatabase() const
    {
        return _spriteDb;
    }

    ////////////////////////////////////////////////////////////////////////////

    Theater::Theater(const Allocator& allocator) :
        _allocator(allocator),
        _impl(_allocator.newItem<Impl>(_allocator), _allocator)
    {

    }

    Theater::~Theater() = default;

    TheaterCLI& Theater::getCLI()
    {
        return *_impl;
    }

    }   // namespace ovengine
}   // namespace cinekine
