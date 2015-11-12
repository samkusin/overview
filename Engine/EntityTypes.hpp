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

#include "EngineTypes.hpp"

#include <ckjson/jsontypes.hpp>
#include <functional>

namespace cinek { namespace ove {

//  Components -----------------------------------------------------------------

struct CameraComponent;
struct RenderableComponent;
struct MeshRenderableComponent;
struct TransformComponent;
struct UpdateTransformComponent;
    
    
enum class ComponentFactoryResult
{
    kSuccess,       ///< Component creation success
    kFailed,        ///< Component creation error
    kPassthrough    ///< Factory method did not apply to the supplied component
};

using CustomComponentCreateFn =
    std::function<void(Entity entity,
                       const cinek::JsonValue& definitions,
                       const char* componentName,
                       const cinek::JsonValue& data)>;

enum
{
    kEntityComponentSpace    = 0x0000,
    kUtilityComponentSpace   = 0x0001,
    kRenderComponentSpace    = 0x0002,
    kCustomComponentSpace0   = 0x8000
};

enum
{
    kRenderableComponent       = MakeComponentId(kRenderComponentSpace,  0x0000),
    kMeshRenderableComponent   = MakeComponentId(kRenderComponentSpace,  0x0001),
    kCameraComponent           = MakeComponentId(kRenderComponentSpace,  0x0002),
    kTransformComponent        = MakeComponentId(kEntityComponentSpace,  0x0000),
    kEntityHierarchyComponent  = MakeComponentId(kEntityComponentSpace,  0x0001)
};


} /* namespace ove */ } /* namespace cinek */

#endif
