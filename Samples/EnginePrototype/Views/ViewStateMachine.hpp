//
//  ViewStateMachine.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/30/16.
//
//

#ifndef Prototype_ViewStateMachine_hpp
#define Prototype_ViewStateMachine_hpp

#include <cinek/types.hpp>
#include <functional>

namespace cinek {
    namespace ove {

struct ViewStateLogic
{
    int id;
    std::function<void()> beginFn;
    std::function<void(CKTimeDelta)> frameUpdateFn;
    std::function<void()> endFn;
};

template<typename StateLogic>
class StateMachine
{
public:
    void setNextState(StateLogic logic);
    void startFrame();
    void runFrameUpdate(CKTimeDelta dt);
    void endFrame();
    
    StateLogic& thisState() {
        return _thisState;
    }
    const StateLogic& thisState() const {
        return _thisState;
    }
    
private:
    StateLogic _thisState;
    StateLogic _nextState;
    bool _nextStatePending = false;
};
    
template<typename StateLogic>
void StateMachine<StateLogic>::setNextState(StateLogic logic)
{
    _nextState = std::move(logic);
    _nextStatePending = true;
}

template<typename StateLogic>
void StateMachine<StateLogic>::startFrame()
{
}

template<typename StateLogic>
void StateMachine<StateLogic>::runFrameUpdate(CKTimeDelta dt)
{
    if (_thisState.frameUpdateFn) {
        _thisState.frameUpdateFn(dt);
    }
}

template<typename StateLogic>
void StateMachine<StateLogic>::endFrame()
{
    if (_nextStatePending) {
        if (_thisState.endFn) {
            _thisState.endFn();
        }
        if (_nextState.beginFn) {
            _nextState.beginFn();
        }
        _thisState = _nextState;
        _nextStatePending = false;
    }
}

using ViewStateMachine = StateMachine<ViewStateLogic>;

    
    } /* namespace ove */
} /* namespace cinek */

#endif /* Prototype_ViewState_hpp */
