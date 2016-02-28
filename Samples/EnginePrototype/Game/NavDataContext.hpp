//
//  NavDatContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 2/17/16.
//
//

#ifndef Game_NavDataContext_hpp
#define Game_NavDataContext_hpp

#include "GameTypes.hpp"

#include "Engine/Nav/NavBody.hpp"

#include <cinek/objectpool.hpp>
#include <unordered_map>

namespace cinek {

class NavDataContext
{
public:
    struct InitParams
    {
        uint32_t navBodyCount;
    };

    NavDataContext(const InitParams& initParams);
    
    ove::NavBody* allocateBody(const ove::NavBody::InitProperties& props);
    ove::NavSceneBodyTransform* allocateTransform(ove::SceneBody* sceneBody, Entity entity);
    ove::NavBody* cloneBody(const ove::NavBody* source, ove::SceneBody* sceneBody, Entity entity);
    void freeBody(ove::NavBody* body);
    
private:
    ObjectPool<ove::NavBody> _navBodyPool;
    ObjectPool<ove::NavSceneBodyTransform> _navSceneBodyPool;
    std::unordered_map<Entity, ove::NavSceneBodyTransform*> _transformMap;
};
    
} /* namespace cinek */

#endif /* Game_NavDataContext_hpp */
