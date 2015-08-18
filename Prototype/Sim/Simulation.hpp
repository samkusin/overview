//
//  AIActionSystem.hpp
//  Overview
//
//  Created by Samir Sinha on 8/14/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_Sim_SimulationConcept_hpp
#define Overview_Sim_SimulationConcept_hpp

#include "SimDefines.hpp"

#include <cinek/vector.hpp>
#include <cinek/debug.hpp>

#include <functional>

namespace cinek { namespace ovproto {

//  Simulation object state enum
//
enum class SimObjectState
{
    kInactive,
    kBegin,
    kUpdate,
    kEnd
};

template<typename Object>
class Simulation
{
public:
    using size_type             = uint32_t;
    using value_type            = Object;
    using key_type              = typename Object::key_type;
    using return_type           = typename Object::return_type;
    using delegate_type         = std::function<void(key_type, return_type)>;
    
    Simulation(size_type objCount, const Allocator& allocator=Allocator());

    // Adds the initialized object into the simulation
    //
    void addObject(value_type&& object, const delegate_type& endDel=delegate_type());
    
    //  Client must have the following implementation
    //      bool begin(Client&, Object& object);
    //      bool update(Client& state, Object& object);
    //      return_type end(Client& state, Object& object);
    //
    template<typename Client>
    void simulate(Client& client, double timeInSecs, double dt);
    
    // O(log N)
    const Object* findObject(key_type id) const;
    Object* findObject(key_type id);
    
private:
    void gc(double time);
    
    struct SimObject
    {
        SimObjectState state;
        value_type obj;
        delegate_type delegateEnd;
        
        SimObject(): state(SimObjectState::kInactive) {}
        SimObject(SimObjectState s, value_type&& o, const delegate_type& d) :
            state(s), obj(std::move(o)), delegateEnd(d)
        {
        }
    };
    
    vector<SimObject> _objects;
};


template<typename Object>
Simulation<Object>::Simulation
(
    size_type objCount,
    const Allocator& alloc
) :
    _objects(alloc)
{
    _objects.reserve(objCount);
}

template<typename Object>
void Simulation<Object>::addObject
(
    value_type&& object,
    const delegate_type& endDel
)
{
    key_type key;
    typename decltype(_objects)::pointer* entry = nullptr;
    
    if (_objects.size() < _objects.capacity())
    {
        //  Take advantage of being a sorted vector.
        //  The last object is the highest object id.
        //  Thus adding an object is trivial (+1 the ID of the last object and
        //  add the new object to the tail.
        key = !_objects.empty() ? _objects.back().obj.id()+1 : 1;
        _objects.emplace_back(SimObjectState::kInactive, object, endDel);
        entry = &_objects.back();
    }
    else
    {
        //  find a free object id if available
        //  if not, then we fail.
        //  note this method should be seldom if ever called since our garbage
        //  collection method should compress the objects vector
        CK_ASSERT(false);
        
        key = 0;
        for (auto& obj : _objects)
        {
            if (obj.state == SimObjectState::kInactive)
            {
                key = entry->obj.id();
                entry->obj = std::move(object);
                entry = &obj;
                break;
            }
        }
    }
    
    CK_ASSERT_RETURN(key > 0 && entry);    // very unlikely
    
    entry->obj.setId(key);
    entry->state = SimObjectState::kBegin;
}

template<typename Object>
template<typename Client>
void Simulation<Object>::simulate
(
    Client& client,
    double time,
    double dt
)
{
    for (size_type i = 0; i < _objects.size(); ++i)
    {
        //  Our client may add to the object list.
        //  Fortunately the object list will never have entries erased (but
        //  cleared.)
        //  But our code may extend the vector (unlikely if we keep to reserved
        //  limits.)  Either way, we access entries by index to take care of
        //  invalidated iterators due to growing vectors.
        //
        auto& simobj = _objects[i];
        
        if (simobj.state == SimObjectState::kBegin)
        {
            if (!client.begin(simobj.obj, time))
                simobj.state = SimObjectState::kEnd;
            else
                simobj.state = SimObjectState::kUpdate;
        }
        if (simobj.state == SimObjectState::kUpdate)
        {
            if (!client.update(simobj.obj, time, dt))
                simobj.state = SimObjectState::kEnd;
        }
        if (simobj.state == SimObjectState::kEnd)
        {
            return_type ret = client.end(simobj.obj, time);
            if (simobj.delegateEnd)
            {
                simobj.delegateEnd(simobj.obj.id(), ret);
            }
            simobj.state = SimObjectState::kInactive;
        }
    }
    
    gc(time);
}

template<typename Object>
void Simulation<Object>::gc(double time)
{
    // TODO!
}

template<typename Object>
auto Simulation<Object>::findObject(key_type id) const -> const Object*
{
    auto it = std::lower_bound(_objects.begin(), _objects.end(), id,
        [](const value_type& simobj, const key_type& id) -> bool {
            return simobj.obj.id() < id;
        });
    if (it == _objects.end() || it->obj.id() != id ||
        it->state == SimObjectState::kInactive)
        return nullptr;
    
    return &(it->obj);
}

template<typename Object>
auto Simulation<Object>::findObject(key_type id) -> Object*
{
    return const_cast<Object*>(static_cast<const Simulation*>(this)->findObject(id));
}

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(Overview_Sim_AIActionSystem_hpp) */
