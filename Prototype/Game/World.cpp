//
//  World.cpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/World.hpp"
#include "Game/WorldObject.hpp"
#include "Core/MessageQueue.hpp"

#include "cinek/objectpool.hpp"
#include "cinek/map.hpp"

#include "btBulletCollisionCommon.h"

#include "Game/Events/SimEventClassIds.hpp"
#include "Game/Events/MoveEntityEvent.hpp"


namespace cinek { namespace overview {

    class World::Impl
    {
    public:
        Impl(const CreateParams& params, const Allocator& allocator);
        ~Impl();
        
        WorldObject* createObject(const Point& pos,
                                  const Point& front,
                                  const AABB<Point>& bbox);
        void destroyObject(WorldObject* body);
        
        void update(MessageQueue& eventQueue, uint32_t deltaTimeMs);

    private:
        Allocator _allocator;
        
        //  Bullet physics world
        btDefaultCollisionConfiguration _btConfig;
        btCollisionDispatcher _btDispatcher;
        btAxisSweep3 _btBroadphase;
        btCollisionWorld _btWorld;
        ObjectPool<btCollisionObject> _objectPool;
        ObjectPool<btBoxShape> _boxPool;
        
        //  Collection of WorldObjects for external usage
        ObjectPool<WorldObject> _worldObjects;
    };

    World::Impl::Impl(const CreateParams& params, const Allocator& allocator) :
        _allocator(allocator),
        _btConfig(),
        _btDispatcher(&_btConfig),
        _btBroadphase(toBtVector3(params.bounds.min), toBtVector3(params.bounds.max)),
        _btWorld(&_btDispatcher, &_btBroadphase, &_btConfig),
        _objectPool(params.objectLimit, _allocator),
        _boxPool(params.objectLimit, _allocator),
        _worldObjects(params.objectLimit, _allocator)
    {

    }

    World::Impl::~Impl()
    {

    }

    WorldObject* World::Impl::createObject(const Point& pos,
                                           const Point& front,
                                           const AABB<Point>& bbox)
    {
        auto boxDims = bbox.dimensions();
        btVector3 boxHalfExt(boxDims.x, boxDims.y, boxDims.z);
        boxHalfExt *= 0.5f;
        btBoxShape* box = _boxPool.construct(boxHalfExt);
        if (box)
        {
            box->setMargin(0.f);
            btCollisionObject* object = _objectPool.construct();
            if (object)
            {
                object->setCollisionShape(box);
                WorldObjectTransform transform = { pos, front, 0.f };
                WorldObject* worldObject = _worldObjects.construct(transform, object);
                if (worldObject)
                {
                    worldObject->writeToTransform(object->getWorldTransform(), boxHalfExt);
                    object->setUserPointer(reinterpret_cast<void*>(worldObject));
                    _btWorld.addCollisionObject(object);
                    return worldObject;
                }
                _objectPool.destruct(object);
            }
            _boxPool.destruct(box);
        }
        
        return nullptr;
    }
    
    void World::Impl::destroyObject(WorldObject* object)
    {
        btCollisionObject* body = reinterpret_cast<btCollisionObject*>(object->context());
        if (body)
        {
            _btWorld.removeCollisionObject(body);
            btCollisionShape* shape = body->getCollisionShape();
        
            switch (shape->getShapeType())
            {
            case BOX_SHAPE_PROXYTYPE:
                {
                    _boxPool.destruct(static_cast<btBoxShape*>(shape));
                }
                break;
            }
        
            body->setCollisionShape(nullptr);
            _objectPool.destruct(body);
        }

        _worldObjects.destruct(object);
    }
    
    void World::Impl::update(MessageQueue& eventQueue, uint32_t deltaTimeMs)
    {
        float deltaTime = deltaTimeMs*0.001f;
        //  physics pass on all btObjects within the btWorld in preparation for
        //  collision detection
        btCollisionObjectArray& btObjects = _btWorld.getCollisionObjectArray();
        for (int objIndex = 0; objIndex < btObjects.size(); ++objIndex)
        {
            btCollisionObject* collObj = btObjects.at(objIndex);
            if (!collObj)
                continue;
            
            //  update our btObject with the new transform
            WorldObject* worldObj =
                reinterpret_cast<WorldObject*>(collObj->getUserPointer());
            btBoxShape* shape =
                static_cast<btBoxShape*>(collObj->getCollisionShape());
            btTransform& btObjTransform = collObj->getWorldTransform();
            worldObj->writeToTransform(btObjTransform,
                                       shape->getHalfExtentsWithoutMargin());
            
            //  apply updated position
            btVector3 nextPos = btObjTransform.getOrigin() +
                (btObjTransform.getBasis() * (kWorldRefDir *
                                              worldObj->transform().speed *
                                              deltaTime));
            btObjTransform.setOrigin(nextPos);
        }
        
        //  perform collision detections
        _btWorld.performDiscreteCollisionDetection();
        
        //  update all WorldObject positions accordingly
        for (int objIndex = 0; objIndex < btObjects.size(); ++objIndex)
        {
            btCollisionObject* collObj = btObjects.at(objIndex);
            if (!collObj)
                continue;
            
            //  update our btObject with the new transform
            WorldObject* worldObj =
                reinterpret_cast<WorldObject*>(collObj->getUserPointer());
            btBoxShape* shape =
                static_cast<btBoxShape*>(collObj->getCollisionShape());
            const btTransform& btObjTransform = collObj->getWorldTransform();
            if (!worldObj->readFromTransform(btObjTransform,
                                             shape->getHalfExtentsWithoutMargin()))
            {
                MoveEntityEvent evt;
                evt.setEntityId(worldObj->id());
                evt.setTransform(worldObj->transform());
                eventQueue.push(SimEvent::kMoveEntity, evt);
            }
        }
    }
    
    
    /////////////////////////////////////////////////////////////////////


    World::World(const CreateParams& params,
                 Allocator& allocator)
    {

        _impl = unique_ptr<Impl>(allocator.newItem<Impl>(params, allocator),
                                 allocator);
    }

    World::~World()
    {
    }
    
    WorldObject* World::createObject(const Point& pos, const Point& front,
                                     const AABB<Point>& bbox)
    {
        return _impl->createObject(pos, front, bbox);
    }
    
    void World::destroyObject(WorldObject* obj)
    {
        _impl->destroyObject(obj);
    }
    
    void World::update(MessageQueue& eventQueue, uint32_t frameTimeMS)
    {
        _impl->update(eventQueue, frameTimeMS);
    }

} /* namespace overview */ } /* namespace cinek */
