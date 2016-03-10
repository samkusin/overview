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
        int containerCount;
        int sequenceCount;
    };
    
    TransformDataContext(InitParams params);
    
    ove::TransformSequence* createSequence();
    void destroySequence(ove::TransformSequence* seq);
    
    ove::TransformContainer* createContainer();
    void destroyContainer(ove::TransformContainer* container);
    
    ove::TransformSetHandle registerSet(ove::TransformSet&& set, std::string name);
    ove::TransformSetHandle findSet(const std::string& name) const;
    
    
private:
    ObjectPool<ove::TransformSequence> _sequencePool;
    ObjectPool<ove::TransformContainer> _containerPool;
    ove::TransformSetPool _setPool;
    ove::TransformSetDictionary _setDict;
};
    
} /* namespace cinek */

#endif /* Overview_Controller_TransformDataContext_hpp */
