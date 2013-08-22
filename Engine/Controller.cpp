//
//  Controller.cpp
//  Overview
//
//  Created by Samir Sinha on 8/10/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Controller.hpp"
#include "Debug.hpp"

#include "Model/SpriteDatabaseLoader.hpp"

#include "cinek/rendermodel/spritedatabase.hpp"

namespace cinekine {
    namespace ovengine {
    
    class Controller::Impl
    {
        friend class Controller;

    public:
        Impl(Allocator& allocator);
        ~Impl();

        
    private:
        Allocator& _allocator;
        rendermodel::SpriteDatabase _spriteDb;

        SpriteDatabaseLoader _spriteDbLoader;
    };
    
    ////////////////////////////////////////////////////////////////////////////
    
    Controller::Impl::Impl(Allocator& allocator) :
        _allocator(allocator),
        _spriteDb(256, allocator),
        _spriteDbLoader(_spriteDb)
    {
    }
    
    Controller::Impl::~Impl()
    {
    }
    

    ////////////////////////////////////////////////////////////////////////////
        
    Controller::Controller(const Allocator& allocator) :
        _allocator(allocator),
        _impl(_allocator.newItem<Impl>(_allocator), SharedPtrDeleter<Impl>(_allocator))
    {
        
    }
    
    Controller::~Controller() = default;
    
    SpriteDatabaseLoader& Controller::getSpriteDatabaseLoader()
    {
        return _impl->_spriteDbLoader;
    }
        
    }   // namespace ovengine
}   // namespace cinekine
