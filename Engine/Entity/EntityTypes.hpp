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

constexpr uint32_t kNullEntity = 0;

inline EntityId makeEntityId(uint16_t shard, uint16_t iteration, uint32_t index) {
    return (((uint64_t)shard) << 48) | (((uint64_t)iteration) << 32) | index;
}

inline uint32_t entityIndexFromId(EntityId eid) {
    //  explicit masking as a hint to compilers that we really want to downcast
    //  this, losing precision.
    return ((uint32_t)eid & 0xffffffff);
}

inline uint16_t entityShardFromId(EntityId eid) {
    return (uint16_t)(eid >> 48);
}

//  Forward decls --------------------------------------------------------------

class EntityDatabase;
class EntityFactory;
class EntityObject;

//  Components -----------------------------------------------------------------

using CustomComponentCreateFn =
    std::function<void(EntityObject& object,
                       const cinek::JsonValue& definitions,
                       const cinek::JsonValue& data)>;

using ComponentId = uint32_t;

using ComponentRowIndex = uint32_t;
    
constexpr ComponentRowIndex kNullComponentRow = UINT32_MAX;

#define COMPONENT_DEFINITION(_type_, _id_) \
    using this_type = _type_; \
    static const ::cinek::overview::ComponentId kComponentId = _id_; \
    static const ::cinek::overview::component::Descriptor kComponentType;

#define COMPONENT_IMPLEMENTATION(_type_) \
    const ::cinek::overview::component::Descriptor _type_::kComponentType =  \
        { _type_::kComponentId, sizeof(_type_) };

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

#endif
