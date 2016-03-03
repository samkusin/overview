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

    
const TransformSequence* TransformContainer::sequence(TransformProperty prop) const
{
    int index = static_cast<int>(prop);
    CK_ASSERT_RETURN_VALUE(index >= static_cast<int>(TransformProperty::kCount), nullptr);
    return &sequences[index];
}
    
    } /* namespace ove */
} /* namespace cinek */
