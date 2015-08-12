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

//  Forward decls --------------------------------------------------------------

class EntityStore;
class EntityObject;
class EntityGroup;
class EntityGroupMap;

//  Components -----------------------------------------------------------------

namespace component
{
    class EntityDataTable;
    struct Camera;
    struct Renderable;
    struct MeshRenderable;
    struct Transform;
    
    template<typename _Component, typename _Container> class Table;
}

using ComponentId = uint32_t;
using ComponentRowIndex = uint32_t;
using EntityGroupMapId = uint32_t;
constexpr ComponentRowIndex kNullComponentRow = UINT32_MAX;

using CustomComponentCreateFn =
    std::function<void(Entity entity,
                       const cinek::JsonValue& definitions,
                       const char* componentName,
                       const cinek::JsonValue& data)>;
    
using CustomComponentDestroyFn =
    std::function<void(Entity entity, ComponentId componentId)>;

#define COMPONENT_DEFINITION(_type_) \
    using this_type = _type_; \
    static const ::cinek::overview::ComponentId kComponentId; \
    static const ::cinek::overview::component::Descriptor kComponentType; \
    static ::cinek::overview::component::MakeDescriptor makeDescriptor( uint32_t cnt); \
    void initialize(::cinek::overview::Entity entity);

#define COMPONENT_TYPE_IMPL(_type_, _id_, _init_body_) \
    void _type_::initialize(::cinek::overview::Entity entity) { \
        _init_body_ \
    } \
    ::cinek::overview::component::MakeDescriptor _type_::makeDescriptor ( uint32_t cnt) { \
        ::cinek::overview::component::MakeDescriptor md; \
        md.desc = kComponentType; \
        md.cnt = cnt; \
        return md; \
    } \
    void _type_ ## initialize_hook(::cinek::overview::Entity e, void* pdata) { \
        _type_::this_type* data = reinterpret_cast<_type_::this_type*>(pdata); \
        data->initialize(e); \
    } \
    const ::cinek::overview::ComponentId _type_::kComponentId = _id_; \
    const ::cinek::overview::component::Descriptor _type_::kComponentType =  \
        { _type_::kComponentId, sizeof(_type_), #_type_,  _type_ ## initialize_hook };

namespace component
{
    struct Descriptor
    {
        ComponentId id;
        size_t recordSize;
        const char* name;
        void (*initCb)(::cinek::overview::Entity, void*);
    };
    
    struct MakeDescriptor
    {
        Descriptor desc;
        uint32_t cnt;
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
        kRenderable         = MakeComponentId(kRenderSpace,  0x0000),
        kMeshRenderable     = MakeComponentId(kRenderSpace,  0x0001),
        kCamera             = MakeComponentId(kRenderSpace,  0x0002),
        kTransform          = MakeComponentId(kEntitySpace,  0x0000),
        kEntityHierarchy    = MakeComponentId(kEntitySpace,  0x0001),
        kEmpty              = MakeComponentId(0xffff,        0xffff)
    };

}

} /* namespace overview */ } /* namespace cinek */

namespace std {
    template<> struct hash<cinek::overview::Entity>
    {
        size_t operator()(const cinek::overview::Entity& entity) const
        {
            return entity.id;
        }
    };
}

#endif
