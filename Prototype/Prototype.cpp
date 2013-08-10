//
//  prototype.cpp
//  Overview
//
//  Created by Samir Sinha on 7/7/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Prototype.hpp"

#include <cstring>



cinekine::ovengine::Game* OVEngine_CreateGame()
{
    return new cinekine::Prototype();
}


///////////////////////////////////////////////////////////////////////////////

#include "cinek/rendermodel/spritedatabase.hpp"

namespace cinekine {

Prototype::ImplPtr::ImplPtr(Impl* ptr) :
    cinekine::unique_ptr<Impl>(ptr)
{
}

//  Main Prototype Implementation
//
class Prototype::Impl
{
public:
    Impl();
    virtual ~Impl();
    
    void update();

private:
    rendermodel::SpriteDatabase _spriteDb;
};


Prototype::Impl::Impl() :
    _spriteDb(256)
{
}

Prototype::Impl::~Impl()
{
        
}

void Prototype::Impl::update()
{
        
}

///////////////////////////////////////////////////////////////////////////////

Prototype::Prototype() :
    _impl(Allocator().newItem<Prototype::Impl>())
{
}

Prototype::~Prototype()
{
}

void Prototype::update()
{
    _impl->update();
}

}   // namespace cinekine
