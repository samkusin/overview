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

using CustomComponentCreateFn =
    std::function<void(Entity entity,
                       const cinek::JsonValue& definitions,
                       const char* componentName,
                       const cinek::JsonValue& data)>;
    
using ComponentId = uint32_t;
using ComponentRowIndex = uint32_t;
constexpr ComponentRowIndex kNullComponentRow = UINT32_MAX;

using EntityGroupMapId = uint32_t;

#define COMPONENT_DEFINITION(_type_, _context_type_) \
    using this_type = _type_; \
    using context_type = _context_type_; \
    static const ::cinek::overview::ComponentId kComponentId; \
    static const ::cinek::overview::component::Descriptor kComponentType; \
    static ::cinek::overview::component::MakeDescriptor makeDescriptor( \
        uint32_t cnt, context_type* context=nullptr); \
    void initialize(::cinek::overview::Entity entity, context_type* context); \
    void terminate(::cinek::overview::Entity entity, context_type* context);

#define COMPONENT_TYPE_IMPL(_type_, _id_, _init_body_, _term_body_ ) \
    void _type_::initialize(::cinek::overview::Entity entity, context_type* context) { \
        _init_body_ \
    } \
    void _type_::terminate(::cinek::overview::Entity entity, context_type* context) { \
        _term_body_ \
    }\
    ::cinek::overview::component::MakeDescriptor _type_::makeDescriptor ( \
        uint32_t cnt, context_type* context) { \
        ::cinek::overview::component::MakeDescriptor md; \
        md.desc = kComponentType; \
        md.cnt = cnt; \
        md.context = reinterpret_cast<void*>(context); \
        return md; \
    } \
    void _type_ ## initialize_hook(::cinek::overview::Entity e, void* pdata, void* context) { \
        _type_::this_type* data = reinterpret_cast<_type_::this_type*>(pdata); \
        data->initialize(e, reinterpret_cast<_type_::context_type*>(context)); \
    } \
    void _type_ ## terminate_hook(::cinek::overview::Entity e, void* pdata, void* context) { \
        _type_::this_type* data = reinterpret_cast<_type_::this_type*>(pdata); \
        data->terminate(e, reinterpret_cast<_type_::context_type*>(context)); \
    }\
    const ::cinek::overview::ComponentId _type_::kComponentId = _id_; \
    const ::cinek::overview::component::Descriptor _type_::kComponentType =  \
        { _type_::kComponentId, sizeof(_type_), _type_ ## initialize_hook, _type_ ## terminate_hook };

namespace component
{
    struct Camera;
    struct Renderable;
    struct MeshRenderable;
    struct Transform;
    
    template<typename _Component, typename _Container> class Table;

    struct Descriptor
    {
        ComponentId id;
        size_t recordSize;
        void (*initCb)(::cinek::overview::Entity, void*, void*);
        void (*termCb)(::cinek::overview::Entity, void*, void*);
    };
    
    struct MakeDescriptor
    {
        Descriptor desc;
        uint32_t cnt;
        void* context;
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
        kRenderable         = MakeComponentId(kRenderSpace, 0x0000),
        kMeshRenderable     = MakeComponentId(kRenderSpace, 0x0001),
        kCamera             = MakeComponentId(kRenderSpace, 0x0002),
        kTransform          = MakeComponentId(kEntitySpace, 0x0000),
        kEntityHierarchy    = MakeComponentId(kEntitySpace, 0x0001),
        kEmpty              = MakeComponentId(0xffff, 0xffff)
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
