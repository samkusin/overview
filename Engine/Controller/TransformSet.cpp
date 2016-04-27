//
//  TransformSet.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/2/16.
//
//

#include "TransformSet.hpp"
#include "Engine/Debug.hpp"

#include <ckm/equation.inl>
#include <ckm/sequence.inl>

namespace ckm {

    template struct equation<ckm::scalar>;
    template class sequence<cinek::ove::TransformProperty, cinek::ove::TransformKeyframe>;
}

namespace cinek {
    namespace ove {
    
TransformSequence::TransformSequence() :
    __prevListNode(nullptr),
    __nextListNode(nullptr)
{
}

TransformSequence::TransformSequence(TransformProperty prop, int kfcount) :
    data(prop, kfcount),
    __prevListNode(nullptr),
    __nextListNode(nullptr)
{
}

TransformAction::TransformAction() :
    __prevListNode(nullptr),
    __nextListNode(nullptr)
{
}

TransformAction::TransformAction(std::string n) :
    name(std::move(n)),
    __prevListNode(nullptr),
    __nextListNode(nullptr)
{
}
    
    } /* namespace ove */
} /* namespace cinek */
