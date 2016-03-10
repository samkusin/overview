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

#include <cinek/intrusive_list.hpp>
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
    kUnknown,
    kCount = kUnknown
};
    
using TransformKeyframe = ckm::keyframe<ckm::scalar>;

struct TransformSequence
{
    TransformSequence();
    TransformSequence(TransformProperty prop, int kfcount);
    
    ckm::sequence<TransformProperty, TransformKeyframe> data;
    
    TransformSequence* __prevListNode;
    TransformSequence* __nextListNode;
};

/**
 *  @struct TransformContainer
 *  @brief  Represents a single transform action
 *  @detail This is a convenience container representing all sequences in
 *          an action.  They can be indexed
 */
struct TransformContainer
{
    TransformContainer();
    TransformContainer(std::string n);
    
    std::string name;
    intrusive_list<TransformSequence> sequenceList;
    
    TransformContainer* __prevListNode;
    TransformContainer* __nextListNode;
};

struct TransformSet
{
    std::string name;
    intrusive_list<TransformContainer> containerList;
};
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_Controller_TransformSet_hpp */
