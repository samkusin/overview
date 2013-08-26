//
//  Theater.hpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Theater.hpp"
#include "TheaterClient.hpp"
#include "SpriteDatabaseLoader.hpp"
#include "TileDatabaseLoader.hpp"

#include "Engine/Debug.hpp"
#include "Engine/Stream/FileStreamBuf.hpp"

#include "cinek/rendermodel/spritedatabase.hpp"
#include "cinek/rendermodel/tiledatabase.hpp"

#include "cinek/core/ckfspath.h"

namespace cinekine {
    namespace ovengine {
    
    class Theater::Impl: public TheaterClient
    {
        friend class Theater;

    public:
        Impl(Allocator& allocator);
        ~Impl();
        
        virtual void loadSpriteDatabase(const char* spriteDbName);
        virtual void loadTileDatabase(const char* tileDbName);
        
    private:
        Allocator& _allocator;
        rendermodel::TileDatabase _tileDb;
        rendermodel::SpriteDatabase _spriteDb;

        TileDatabaseLoader _tileDbLoader;
        SpriteDatabaseLoader _spriteDbLoader;
    };
    
    ////////////////////////////////////////////////////////////////////////////
    
    Theater::Impl::Impl(Allocator& allocator) :
        _allocator(allocator),
        _tileDb(1024, allocator),
        _spriteDb(1024, allocator),
        _tileDbLoader(_tileDb),
        _spriteDbLoader(_spriteDb)
    {
    }
    
    Theater::Impl::~Impl()
    {
    }
    
    void Theater::Impl::loadSpriteDatabase(const char* spriteDbName)
    {
        char imagePath[MAX_PATH];
        snprintf(imagePath, sizeof(imagePath), "data/%s.json", spriteDbName);
        FileStreamBuf dbStream(imagePath);
        if (!dbStream)
            return;
        
        _spriteDbLoader.unserialize(dbStream);
    }

    void Theater::Impl::loadTileDatabase(const char* tileDbName)
    {
        char imagePath[MAX_PATH];
        snprintf(imagePath, sizeof(imagePath), "data/%s.json", tileDbName);
        FileStreamBuf dbStream(imagePath);
        if (!dbStream)
            return;
        
        _tileDbLoader.unserialize(dbStream);
    }

    ////////////////////////////////////////////////////////////////////////////
        
    Theater::Theater(const Allocator& allocator) :
        _allocator(allocator),
        _impl(_allocator.newItem<Impl>(_allocator), SharedPtrDeleter<Impl>(_allocator))
    {
        
    }
    
    Theater::~Theater() = default;
    
    SpriteDatabaseLoader& Theater::getSpriteDatabaseLoader()
    {
        return _impl->_spriteDbLoader;
    }
    
    TileDatabaseLoader& Theater::getTileDatabaseLoader()
    {
        return _impl->_tileDbLoader;
    }

    TheaterClient& Theater::getClient()
    {
        return *_impl;
    }
        
    }   // namespace ovengine
}   // namespace cinekine
