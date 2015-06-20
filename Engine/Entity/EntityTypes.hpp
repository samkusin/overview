///
/// @file Entity/EntityTypes.hpp
/// Contains common definitions for an Entity Component System Framework
///
/// @author     Samir Sinha
/// @date       5/1/15
/// @copyright  Copyright 2015 Cinekine. All rights reserved.
/// @license    The MIT License
///

#ifndef Overview_Entity_Types_hpp
#define Overview_Entity_Types_hpp

#include "Engine/EngineTypes.hpp"

#include <cinek/types.hpp>
#include <cinek/json/jsontypes.hpp>
#include <functional>

namespace cinek { namespace overview {

struct Entity
{
    using value_type = uint64_t;
    using index_type = uint32_t;
    using iteration_type = uint16_t;
    static constexpr value_type null_value = 0;
    static Entity null()  { Entity eid; eid.id = 0; return eid; }
    
    void makeEntityId(iteration_type iteration, index_type index) {
        id = (((value_type)iteration) << 48) | index;
    }
    
    operator bool() const {
        return id != null_value;
    }
    Entity& operator=(value_type val) {
        id = val;
        return *this;
    }
    
    value_type value() const { return id; }
    index_type index() const { return ((index_type)id & 0xffffffff); }
    iteration_type iteration() const { return (iteration_type)(id >> 48); }
    
    value_type id;
};

//  Forward decls --------------------------------------------------------------

class EntityDatabase;
class EntityFactory;
class EntityObject;

//  Components -----------------------------------------------------------------

using CustomComponentCreateFn =
    std::function<void(Entity entity,
                       const cinek::JsonValue& definitions,
                       const cinek::JsonValue& data)>;
    
using CustomComponentDestroyFn =
    std::function<void(Entity entity)>;

using ComponentId = uint32_t;

using ComponentRowIndex = uint32_t;
    
constexpr ComponentRowIndex kNullComponentRow = UINT32_MAX;

#define COMPONENT_DEFINITION(_type_) \
    using this_type = _type_; \
    static const ::cinek::overview::ComponentId kComponentId; \
    static const ::cinek::overview::component::Descriptor kComponentType;

#define COMPONENT_TYPE_IMPL(_type_, _id_, _init_body_, _term_body_ ) \
    void _type_ ## initialize(_type_* data) { \
        _init_body_ \
    } \
    void _type_ ## terminate(_type_* data) { \
        _term_body_ \
    }\
    void _type_ ## initialize_hook(void* pdata) { \
        _type_::this_type* data = reinterpret_cast<_type_::this_type*>(pdata); \
        _type_ ## initialize(data); \
    } \
    void _type_ ## terminate_hook(void* pdata) { \
        _type_::this_type* data = reinterpret_cast<_type_::this_type*>(pdata); \
        _type_ ## terminate(data); \
    }\
    const ::cinek::overview::ComponentId _type_::kComponentId = _id_; \
    const ::cinek::overview::component::Descriptor _type_::kComponentType =  \
        { _type_::kComponentId, sizeof(_type_), _type_ ## initialize_hook, _type_ ## terminate_hook };


namespace component
{
    struct Light;
    struct Camera;
    struct Renderable;
    struct Transform;   

    struct Descriptor
    {
        ComponentId id;
        size_t recordSize;
        void (*initCb)(void*);
        void (*termCb)(void*);
    };

    enum
    {
        kEntitySpace    = 0x0000,
        kUtilitySpace   = 0x0001,
        kRenderSpace    = 0x0002,
        kCustomSpace0   = 0x8000
    };

    constexpr ComponentId MakeComponentId(uint16_t category, uint16_t id)
    {
        return (((ComponentId)category) << 16) | id;
    }

    enum
    {
        kLight      = MakeComponentId(kRenderSpace, 0x0001),
        kCamera     = MakeComponentId(kRenderSpace, 0x0002),
        kTransform  = MakeComponentId(kUtilitySpace,0x0000),
        kRenderable = MakeComponentId(kEntitySpace, 0x0000),
        kEmpty      = MakeComponentId(0xffff, 0xffff)
    };

}

} /* namespace overview */ } /* namespace cinek */

namespace std {
    template<> struct hash<cinek::overview::Entity>
    {
        size_t operator()(const cinek::overview::Entity& entity) const
        {
            return entity.index();
        }
    };
}

#endif
