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
        struct RenderContext;
        
        template<typename Object> class Component;
        template<typename Body, typename Derived> class System;
        
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
        class NavPath;
        class NavPathQueryPool;
        class NavPathQuery;
        
        struct TransformAction;
        struct TransformSet;
        class TransformBody;
        class TransformSystem;
        class NavBody;
        class NavBodyTransform;
        class NavSceneBodyTransform;
        class NavSystem;
        
        class AssetService;
        class EntityService;
        
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
