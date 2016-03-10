//
//  TransformSetJsonLoader.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/3/16.
//
//

#ifndef Game_TransformSetJsonLoader_hpp
#define Game_TransformSetJsonLoader_hpp

#include "GameTypes.hpp"

#include "Engine/Controller/TransformSet.hpp"

#include <ckjson/jsontypes.hpp>

namespace cinek {

ove::TransformSet loadTranformSetFromJSON
(
    TransformDataContext& context,
    const JsonValue& json
);

ove::TransformContainer loadTransformContainerFromJSON
(
    TransformDataContext& context,
    const JsonValue& json
);

ove::TransformSequence loadTransformSequenceFromJSON
(
    TransformDataContext& context,
    ove::TransformProperty property,
    const JsonValue& json
);


} /* namespace cinek */

#endif /* Game_TransformSetJsonLoader_hpp */
