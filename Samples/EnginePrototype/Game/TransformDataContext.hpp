//
//  TransformDataContext.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/2/16.
//
//

#ifndef Game_TransformDataContext_hpp
#define Game_TransformDataContext_hpp

#include "GameTypes.hpp"
#include "Engine/Controller/ControllerTypes.hpp"
#include "Engine/Controller/TransformSet.hpp"

namespace cinek {
    
class TransformDataContext
{
public:
    struct InitParams
    {
        int setCount;
        int actionCount;
        int sequenceCount;
        int bodyCount;
    };
    
    TransformDataContext(InitParams params);
    
    ove::TransformSequence* allocateSequence();
    void freeSequence(ove::TransformSequence* seq);
    
    ove::TransformAction* allocateAction();
    void freeAction(ove::TransformAction* container);
    
    ove::TransformSetHandle registerSet(ove::TransformSet&& set, std::string name);
    ove::TransformSetHandle findSet(const std::string& name) const;
    
    ove::TransformBody* allocateBody(Entity entity, ove::TransformSetHandle setHandle);
    void freeBody(ove::TransformBody* body);
    
private:
    ObjectPool<ove::TransformSequence> _sequencePool;
    ObjectPool<ove::TransformAction> _actionPool;
    ove::TransformSetPool _setPool;
    ove::TransformSetDictionary _setDict;
    
    ObjectPool<ove::TransformBody> _bodyPool;
};
    
} /* namespace cinek */

#endif /* Overview_Controller_TransformDataContext_hpp */
