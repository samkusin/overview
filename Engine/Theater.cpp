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
#include "cinek/overview/map.hpp"

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
        virtual void setViewMap(std::shared_ptr<overview::Map> map, const cinek_ov_pos& pos);
        virtual void setViewPos(const cinek_ov_pos& pos);
        virtual void clearViewMap();

    private:
        rendermodel::TileDatabase _tileDb;
        TileDatabaseLoader _tileDbLoader;
        rendermodel::SpriteDatabase _spriteDb;
        SpriteDatabaseLoader _spriteDbLoader;

        std::function<void(std::shared_ptr<overview::Map>&, const cinek_ov_pos&)> _onViewMapSet;
        std::function<void(const cinek_ov_pos&)> _ovViewPosSet;
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
        char imagePath[MAX_PATH];
        snprintf(imagePath, sizeof(imagePath), "data/%s.json", spriteDbName);
        FileStreamBuf dbStream(imagePath);
        if (!dbStream)
            return;

        _spriteDbLoader.onBitmapAtlasRequest([&bitmapLibrary]
            (const char* atlasName) -> cinek_bitmap_atlas
            {
                return bitmapLibrary.loadAtlas(atlasName);
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
        char imagePath[MAX_PATH];
        snprintf(imagePath, sizeof(imagePath), "data/%s.json", tileDbName);
        FileStreamBuf dbStream(imagePath);
        if (!dbStream)
            return;

        _tileDbLoader.onBitmapAtlasRequest([&bitmapLibrary]
            (const char* atlasName) -> cinek_bitmap_atlas
            {
                return bitmapLibrary.loadAtlas(atlasName);
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

    void Theater::Impl::setViewMap(std::shared_ptr<overview::Map> map, const cinek_ov_pos& pos)
    {
        _onViewMapSet(map, pos);
    }

    void Theater::Impl::setViewPos(const cinek_ov_pos &pos)
    {
        _ovViewPosSet(pos);
    }

    void Theater::Impl::clearViewMap()
    {
        std::shared_ptr<overview::Map> nilMap;
        _onViewMapSet(nilMap, glm::vec3());
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
