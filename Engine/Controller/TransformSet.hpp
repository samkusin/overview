//
//  TransformSet.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/2/16.
//
//

#ifndef Overview_Controller_TransformSet_hpp
#define Overview_Controller_TransformSet_hpp

#include "ControllerTypes.hpp"

#include <ckm/keyframe.hpp>
#include <ckm/sequence.hpp>

#include <string>

namespace cinek {
    namespace ove {

enum class TransformProperty : int
{
    kX,
    kY,
    kZ,
    kQX,
    kQY,
    kQZ,
    kQW,
    kCount
};
    
using TransformKeyframe = ckm::keyframe<ckm::scalar>;
using TransformSequence = ckm::sequence<TransformProperty, TransformKeyframe>;

/**
 *  @struct TransformContainer
 *  @brief  Represents a single transform action
 *  @detail This is a convenience container representing all sequences in
 *          an action.  They can be indexed
 */
struct TransformContainer
{
    const TransformSequence* sequences;
    int sequenceCount;
    
    const TransformSequence* sequence(TransformProperty prop) const;
};

struct TransformSet
{
    std::string name;
    TransformContainer* containers;
    int containerCount;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_Controller_TransformSet_hpp */
