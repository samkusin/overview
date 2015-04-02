//
//  World.cpp
//  Overview
//
//  Created by Samir Sinha on 10/12/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "Game/World.hpp"
#include "Game/WorldObject.hpp"
#include "Game/WorldMap.hpp"
#include "Shared/GameTemplates.hpp"
#include "Shared/StaticWorldMap.hpp"
#include "Core/MessageQueue.hpp"
#include "Engine/Model/TileGridMap.hpp"
#include "Engine/Model/TileLibrary.hpp"

#include "cinek/objectpool.hpp"
#include "cinek/map.hpp"

#include "btBulletCollisionCommon.h"
#include "LinearMath/btIDebugDraw.h"

#include "Game/Events/SimEventClassIds.hpp"
#include "Game/Events/MoveEntityEvent.hpp"

#include "cinek/debug.hpp"



namespace cinek { namespace overview {

    class WorldDebugDraw : public btIDebugDraw
    {
        CK_CLASS_NON_COPYABLE(WorldDebugDraw);
        
    public:
        WorldDebugDraw(const Allocator& allocator) :
            _debugMode(0),
            _messages(allocator)
        {
            _messages.reserve(4096);
        }
        
        virtual ~WorldDebugDraw()
        {
        }
        
        virtual void drawLine(const btVector3& from,
            const btVector3& to,
            const btVector3& color)
        {
            _messages.emplace_back();
            _messages.back().type = SimDebugMessage::kDrawLine;
            _messages.back().p0 = toPoint(from);
            _messages.back().p1 = toPoint(to);
            _messages.back().color = toPoint(color);
        }
        
    	virtual void drawContactPoint(const btVector3& PointOnB,
            const btVector3& normalOnB,
            btScalar distance,
            int lifeTime,
            const btVector3& color)
        {
            _messages.emplace_back();
            _messages.back().type = SimDebugMessage::kDrawPoint;
            _messages.back().p0 = toPoint(PointOnB);
            _messages.back().color = toPoint(color);
        }

        virtual void reportErrorWarning(const char* warningString)
        {
            CK_LOG_WARN("Simulation::World", warningString);
        }

        virtual void draw3dText(const btVector3& ,const char* )
        {
        }
	
        virtual void setDebugMode(int debugMode)
        {
            _debugMode = debugMode;
        }
	
        virtual int	getDebugMode() const
        {
            return _debugMode;
        }
        
        void appendDebugMessages(vector<SimDebugMessage>& msgs)
        {
            msgs.insert(msgs.end(),
                std::make_move_iterator(_messages.begin()),
                std::make_move_iterator(_messages.end()));
            
            _messages.clear();
        }
        
    private:
        int _debugMode;
        vector<SimDebugMessage> _messages;
    };
    
    /////////////////////////////////////////////////////////////////////

    class World::Impl
    {
        CK_CLASS_NON_COPYABLE(Impl);
        
    public:
        Impl(const CreateParams& params, const Allocator& allocator);
        ~Impl();
        
        WorldObject* createObject(const Point& pos,
                                  const Point& front,
                                  const std::string& modelName);
        void destroyObject(WorldObject* body);
        
        void update(MessageQueue& eventQueue, uint32_t deltaTimeMs);
        
        WorldDebugDraw* debugDraw() { return _debugDraw.get(); }
        
    private:
        btBoxShape* constructBox(const AABB<Point>& aabb);
        
        void addWorldObjectToTransformList(WorldObjectBase* obj);
        
        //void collideObjectWithStatic(const btCollisionObject)

    private:
        Allocator _allocator;
        const GameTemplates* _gameTemplates;
        
        //  Collection of WorldObjects for external usage
        ObjectPool<WorldObject> _worldObjects;
        
        // Bullet objects for btWorld usage
        ObjectPool<btCollisionObject> _objectPool;
        ObjectPool<btBoxShape> _boxPool;
        
        unique_ptr<WorldDebugDraw> _debugDraw;
        
        //  Map data
        WorldMap _worldMap;
        
        struct Shape
        {
            uint32_t slot;
            uint32_t index;
            btBoxShape* shape;
            bool operator<(const Shape& other) const {
                return slot < other.slot || (slot == other.slot && index < other.index);
            }
        };
        
        vector<Shape> _tileShapes;
        vector<Shape> _spriteShapes;
        
        vector<WorldObjectBase*> _changedObjects;
        
        //  Bullet physics world
        btDefaultCollisionConfiguration _btConfig;
        btCollisionDispatcher _btDispatcher;
        btAxisSweep3 _btBroadphase;
        btCollisionWorld _btWorld;
    };

    World::Impl::Impl(const CreateParams& params, const Allocator& allocator) :
        _allocator(allocator),
        _gameTemplates(params.gameTemplates),
        _worldObjects(params.objectLimit, _allocator),
        _objectPool(params.objectLimit, _allocator),
        _boxPool(params.objectLimit, _allocator),
        _worldMap(*params.staticWorldMap),
        _tileShapes(_allocator),
        _spriteShapes(_allocator),
        _changedObjects(_allocator),
        _btConfig(),
        _btDispatcher(&_btConfig),
        _btBroadphase(toBtVector3(_worldMap.bounds().min),
                      toBtVector3(_worldMap.bounds().max)),
        _btWorld(&_btDispatcher, &_btBroadphase, &_btConfig)
    {
        _debugDraw = allocate_unique<WorldDebugDraw>(_allocator, _allocator);
        _btWorld.setDebugDrawer(_debugDraw.get());
        
        if (params.visualDebug)
        {
            _btWorld.getDebugDrawer()->setDebugMode(WorldDebugDraw::DBG_DrawWireframe);
        }
        
        //  Tile BoxShapes
        auto& tileLibrary = params.staticWorldMap->tileLibrary();
        _tileShapes.reserve(tileLibrary.totalCount());
        tileLibrary.forEach(
            [this](TileLibrary::SlotType slot, const TileCollection& collection) -> bool
            {
                for (uint32_t index = 0; index < collection.count(); ++index)
                {
                    auto& tile = collection[index];
                    if (tile.collision.shape == CollisionInfo::Shape::kNone)
                        continue;
                    
                    _tileShapes.emplace_back();
                    auto& tileShape = _tileShapes.back();
                    tileShape.slot = slot;
                    tileShape.index = index;
                    if (tile.collision.shape == CollisionInfo::Shape::kBox)
                    {
                        tileShape.shape = constructBox(tile.aabb);
                    }
                    else
                    {
                        tileShape.shape = nullptr;
                    }
                }
                return true;
            });
        std::sort(_tileShapes.begin(), _tileShapes.end());

        //  Sprite BoxShapes
        auto& spriteCollection = params.staticWorldMap->spriteCollection();
        _spriteShapes.reserve(spriteCollection.count());
        for (uint32_t index = 0; index < spriteCollection.count(); ++index)
        {
            auto& sprite = spriteCollection[index];
            _spriteShapes.emplace_back();
            auto& shape = _spriteShapes.back();
            shape.slot = 0;
            shape.index = index;
            shape.shape = constructBox(sprite.aabb());
        }
        std::sort(_spriteShapes.begin(), _spriteShapes.end());
        
        // Using the StaticWorldMap, add btCollisionObjects for every collidible
        // tile.
        //
        // Phase 1: Overlay
        // Phase 2: Terrain Triangle Mesh
        // Phase 3: Merge Overlay boxes into Concave Shape (Triangle Meshes)
        //
        auto staticWorldMap = params.staticWorldMap;
        auto& overlay = staticWorldMap->tileGridMap().overlay();
        Point mapTilePos;
        for (int row = 0; row < overlay.rowCount(); ++row)
        {
            mapTilePos.x = 1.0f;
            mapTilePos.y = row + 1.0f;
            mapTilePos.z = 0;
            
            auto strip = overlay.atRow(row, 0);
            for (auto tilep = strip.first; tilep != strip.second; ++tilep, mapTilePos.x += 1.0f)
            {
                TileId tileId = *tilep;
                if (!tileId)
                    continue;
                auto tileShapeIt = std::lower_bound(
                    _tileShapes.begin(),
                    _tileShapes.end(),
                    tileId,
                    [](const Shape& shape, TileId tileId) -> bool {
                        return compressTileToId(shape.slot, shape.index) < tileId;
                    });
                if (tileShapeIt == _tileShapes.end() ||
                    tileShapeIt->slot != slotFromTileId(tileId) ||
                    tileShapeIt->index != indexFromTileId(tileId))
                    continue;
              
                btBoxShape* shape = tileShapeIt->shape;
                if (shape)
                {
                    shape->setMargin(0.f);
                    btCollisionObject* object = _objectPool.construct();
                    if (object)
                    {
                        object->setCollisionShape(shape);
                        object->setUserPointer(reinterpret_cast<void*>(&_worldMap));
                        
                        btTransform transform;
                        //transform.setBasis(orientToBtDirection(kObjectRefDir));
                        transform.setBasis(btMatrix3x3::getIdentity());
                        transform.setOrigin(translateToBtPosition(mapTilePos, shape->getHalfExtentsWithoutMargin()));
                        object->setWorldTransform(transform);
                        _btWorld.addCollisionObject(object,
                                    btBroadphaseProxy::StaticFilter,
                                    btBroadphaseProxy::KinematicFilter);
                    }
                }
            }
        }
        
        //  pre-reserve space for our changed objects vector - which contains all
        //  changed worldobjects within an update()
        _changedObjects.reserve(params.objectLimit);
    }

    World::Impl::~Impl()
    {
        //  all btObjects and WorldObjects reside in pools that are destroyed
        //  after btWorld is destroyed.
        //  so this constructor might be trivial.
    
        _btWorld.setDebugDrawer(nullptr);
    }

    btBoxShape* World::Impl::constructBox(const AABB<Point>& aabb)
    {
        auto boxDims = aabb.dimensions();
        btVector3 boxHalfExt(boxDims.x, boxDims.y, boxDims.z);
        boxHalfExt *= 0.5f;
        return _boxPool.construct(boxHalfExt);
    }

    WorldObject* World::Impl::createObject(const Point& pos,
                                           const Point& front,
                                           const std::string& name)
    {
        int spriteIndex = _gameTemplates->spriteCollection().index(name);
        if (spriteIndex < 0)
            return nullptr;
        
        auto it = std::lower_bound(_spriteShapes.begin(), _spriteShapes.end(),
                    (uint32_t)spriteIndex,
                    [](const Shape& shape, uint32_t spriteIndex) -> bool {
                        return shape.index < spriteIndex;
                    });
        
        btBoxShape* box = nullptr;
        if (it != _spriteShapes.end() && it->index == spriteIndex)
            box = it->shape;
        
        if (box)
        {
            const btVector3& boxHalfExt = box->getHalfExtentsWithoutMargin();
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
                    object->setCollisionFlags(object->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
                    object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                    _btWorld.addCollisionObject(object,
                        btBroadphaseProxy::KinematicFilter,
                        btBroadphaseProxy::StaticFilter | btBroadphaseProxy::KinematicFilter);
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
    
    void World::Impl::addWorldObjectToTransformList(WorldObjectBase* obj)
    {
        if (!obj->testUserFlag(WorldObjectBase::kFlag_Transformed))
        {
            _changedObjects.push_back(obj);
            obj->setUserFlags(WorldObjectBase::kFlag_Transformed);
        }
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
            WorldObjectBase* extObj = reinterpret_cast<WorldObjectBase*>(collObj->getUserPointer());
            if (extObj->classId() == WorldObjectBase::ClassId::kWorldObject)
            {
                WorldObject* worldObj = static_cast<WorldObject*>(extObj);
                btBoxShape* shape =
                    static_cast<btBoxShape*>(collObj->getCollisionShape());
                btTransform &btObjTransform = collObj->getWorldTransform();
                worldObj->writeToTransform(btObjTransform,
                                           shape->getHalfExtentsWithoutMargin());
                
                //  apply updated position
                const WorldObjectTransform& objTransform = worldObj->transform();
                btVector3 nextPos = btObjTransform.getOrigin() +
                    (btObjTransform.getBasis() * (kWorldRefDir *
                                                  objTransform.speed *
                                                  deltaTime));

                if (nextPos != btObjTransform.getOrigin() || objTransform.isDirty())
                {
                    btObjTransform.setOrigin(nextPos);
                    collObj->setWorldTransform(btObjTransform);
                    addWorldObjectToTransformList(worldObj);
                }
            }
        }
       
        /*
        //  perform collision detections for collision based sim.
        _btWorld.performDiscreteCollisionDetection();
        */
        
        //  perform computations for raycasting based sim
        _btWorld.computeOverlappingPairs();
        
        
        //  update all WorldObject positions accordingly
        for (auto& object : _changedObjects)
        {
            if (object->classId() == WorldObjectBase::ClassId::kWorldObject)
            {
                btCollisionObject* collObj = reinterpret_cast<btCollisionObject*>(object->context());
                //  update our btObject with the new transform
                WorldObject* worldObj =
                    reinterpret_cast<WorldObject*>(object);
                if (worldObj->testUserFlag(WorldObjectBase::kFlag_Transformed))
                {
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
                    worldObj->clearUserFlags(WorldObjectBase::kFlag_Transformed);
                }
            }
        }
        
        _changedObjects.clear();
        _btWorld.debugDrawWorld();
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
                                     const std::string& spriteName)
    {
        return _impl->createObject(pos, front, spriteName);
    }
    
    void World::destroyObject(WorldObject* obj)
    {
        _impl->destroyObject(obj);
    }
    
    void World::update(MessageQueue& eventQueue, uint32_t frameTimeMS)
    {
        _impl->update(eventQueue, frameTimeMS);
    }
    
    void World::appendDebugMessages(vector<SimDebugMessage>& output)
    {
        _impl->debugDraw()->appendDebugMessages(output);
    }

} /* namespace overview */ } /* namespace cinek */
