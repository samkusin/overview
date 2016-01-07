//
//  SceneJsonLoader.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 12/5/15.
//
//

#ifndef Overview_SceneObjectJsonLoader_hpp
#define Overview_SceneObjectJsonLoader_hpp

#include "SceneTypes.hpp"

#include <ckjson/json.hpp>

#include <vector>

namespace cinek {
    namespace ove {
    
struct SceneObjectJsonLoader
{
    SceneDataContext* context;
    
    JsonValue::ConstMemberIterator jsonHullsArrayIt;
    
    std::vector<SceneFixedBodyHull*> hulls;
    
    btBvhTriangleMeshShape* operator()(const JsonValue& jsonNode);
};

SceneFixedBodyHull* loadSceneFixedBodyHullFromJSON
(
    SceneDataContext& context,
    const JsonValue& root
);

btVector3& loadVectorFromJSON(btVector3& vec, const JsonValue& vecObj);
    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Overview_SceneObjectJsonLoader_hpp */
