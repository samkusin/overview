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
#include "cinek/overview/map.hpp"

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
        virtual void setViewMap(std::shared_ptr<overview::Map> map, const cinek_ov_pos& pos);
        virtual void setViewPos(const cinek_ov_pos& pos);
        virtual void clearViewMap();
        
    private:
        Allocator& _allocator;
        
        rendermodel::TileDatabase _tileDb;
        TileDatabaseLoader _tileDbLoader;
        rendermodel::SpriteDatabase _spriteDb;
        SpriteDatabaseLoader _spriteDbLoader;
        
        std::function<void(std::shared_ptr<overview::Map>&, const cinek_ov_pos&)> _onViewMapSet;
        std::function<void(const cinek_ov_pos&)> _ovViewPosSet;
    };
    
    ////////////////////////////////////////////////////////////////////////////
    
    Theater::Impl::Impl(Allocator& allocator) :
        _allocator(allocator),
        _tileDb(1024, allocator),
        _tileDbLoader(_tileDb),
        _spriteDb(1024, allocator),
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
    
    const rendermodel::TileDatabase& Theater::getTileDatabase() const
    {
        return _impl->_tileDb;       
    }
    
    const rendermodel::SpriteDatabase& Theater::getSpriteDatabase() const
    {
        return _impl->_spriteDb;
    }
    
    void Theater::onViewMapSet(std::function<void(std::shared_ptr<overview::Map>&, const cinek_ov_pos&)> setCb)
    {
        _impl->_onViewMapSet = setCb;
    }
    
    void Theater::onViewPosSet(std::function<void(const cinek_ov_pos&)> setCb)
    {
        _impl->_ovViewPosSet = setCb;
    }

        
    }   // namespace ovengine
}   // namespace cinekine
