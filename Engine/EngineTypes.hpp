///
/// @file
/// Header documentation for an engine component
///
/// @author     Samir Sinha
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Engine_hpp
#define Overview_Engine_hpp

#include <ckentity/entity.h>
#include <cinek/types.hpp>
#include <ckm/mathtypes.hpp>

namespace cinek {
    namespace ove {

        class EntityDatabase;
        class EntityComponentFactory;

        class ViewStack;
        class ViewController;
        
        class RenderGraph;
        
        class Scene;
        struct SceneBody;
        class SceneFixedBodyHull;
        class SceneDataContext;
        class SceneMotionState;
        class SceneDebugDrawer;
        struct SceneObjectJsonLoader;
        
        class Pathfinder;
        struct PathfinderDebug;
        class PathfinderListener;
        class NavMesh;
        class NavBody;
        class NavBodyTransform;
        class NavSceneBodyTransform;
        class NavSystem;
        class NavPath;
        class NavPathQueryPool;
        class NavPathQuery;
        
        class AssetService;
        class EntityService;
        class RenderService;
        class SceneService;
        
        class AssetManifest;
        class AssetManifestLoader;
        class AssetManfiestFactory;
        
        enum class AssetType
        {
            kNone,
            kTexture,
            kModelSet
        };
    } /* namespace ove */
} /* namespace cinek */

#endif
