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

namespace cinek {
    namespace ove {

        class EntityDatabase;
        class EntityComponentFactory;
        class EntityService;
        
        class ViewStack;
        class ViewController;
        class ViewService;
        
        class Scene;
        class SceneFixedBodyHull;
        class SceneTriangleMeshShape;
        class SceneBody;
        class SceneDataContext;
        
        class SceneService;
        class ViewAPI;
        
        class AssetManifest;
        class AssetManifestLoader;
        class AssetManfiestFactory;
        
        enum class AssetType
        {
            kNone,
            kTexture,
            kModel
        };

    } /* namespace ove */
} /* namespace cinek */

#endif
