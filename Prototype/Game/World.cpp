//
//  World.cpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/World.hpp"

#include "btBulletCollisionCommon.h"


namespace cinek { namespace overview {

    class World::Impl
    {
    public:
        Impl(const WorldAABB& bounds);
        ~Impl();

    private:
        btDefaultCollisionConfiguration _btConfig;
        btCollisionDispatcher _btDispatcher;
        btAxisSweep3 _btBroadphase;
        btCollisionWorld _btWorld;
    };

    World::Impl::Impl(const AABB<Point>& bounds) :
        _btConfig(),
        _btDispatcher(&_btConfig),
        _btBroadphase(toBtVector3(bounds.min), toBtVector3(bounds.max)),
        _btWorld(&_btDispatcher, &_btBroadphase, &_btConfig)
    {

    }

    World::Impl::~Impl()
    {

    }

    /////////////////////////////////////////////////////////////////////


    World::World(const WorldAABB& bounds,
                 const Allocator& allocator) :
        _allocator(allocator)
    {

        _impl = unique_ptr<Impl>(_allocator.newItem<Impl>(bounds),
                                 _allocator);
    }

    World::~World()
    {
    }

} /* namespace overview */ } /* namespace cinek */
