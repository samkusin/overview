//
//  TransformSetJsonLoader.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 3/3/16.
//
//

#include "TransformSetJsonLoader.hpp"
#include "TransformDataContext.hpp"

#include <ckjson/json.hpp>


namespace cinek {

/*
    Example:
    
    "animation" : {
        "set": {
            "open" : {
                "qx" : [ { "t": 0, "v": 0 }, { "t": 1.0, "v": 0 } ],
                "qy" : [ { "t": 0, "v": 0 }, { "t": 1.0, "v": 0.70710678 } ],
                "qz" : [ { "t": 0, "v": 0 }, { "t": 1.0, "v": 0 } ],
                "qw" : [ { "t": 0, "v": 0 }, { "t": 1.0, "v": -0.70710678 } ]
            },
            "close" : {
                "qx" : [ { "t": 0, "v": 0 }, { "t": 1.0, "v": 0 } ],
                "qy" : [ { "t": 0, "v": 0.70710678 }, { "t": 1.0, "v": 0 } ],
                "qz" : [ { "t": 0, "v": 0 }, { "t": 1.0, "v": 0 } ],
                "qw" : [ { "t": 0, "v": -0.70710678 }, { "t": 1.0, "v": 0 } ]
            }
        }
    }
 */
 
ove::TransformProperty fromTransformSequenceKeyName(const char* key)
{
    static const char* names[] = {
        "tx",
        "ty",
        "tz",
        "qx",
        "qy",
        "qz",
        "qw"
    };
    
    for (int i = 0; i < sizeof(names)/sizeof(names[0]); ++i) {
        if (!strcasecmp(names[i], key)) {
            CK_ASSERT(i < static_cast<int>(ove::TransformProperty::kCount));
            return static_cast<ove::TransformProperty>(i);
        }
    }
    
    return ove::TransformProperty::kUnknown;
}

ove::TransformSet loadTranformSetFromJSON
(
    TransformDataContext& context,
    const JsonValue& json
)
{
    ove::TransformSet set;
    
    for (auto memberIt = json.MemberBegin();
         memberIt != json.MemberEnd();
         ++memberIt) {
        
        auto container = context.createContainer();
        container->name = memberIt->name.GetString();
        *container = std::move(loadTransformContainerFromJSON(context, memberIt->value));
        
        set.containerList.push_back(container);
    }
    
    return set;
}

ove::TransformContainer loadTransformContainerFromJSON
(
    TransformDataContext& context,
    const JsonValue& json
)
{
    ove::TransformContainer container;
    
    for (auto memberIt = json.MemberBegin();
         memberIt != json.MemberEnd();
         ++memberIt) {
        auto& value = memberIt->value;
        if (value.IsArray()) {
            auto sequence = context.createSequence();
            
            *sequence = loadTransformSequenceFromJSON(
                context,
                fromTransformSequenceKeyName(memberIt->name.GetString()),
                value);
        
            container.sequenceList.push_back(sequence);
        }
    }
    
    return container;
}

ove::TransformSequence loadTransformSequenceFromJSON
(
    TransformDataContext& context,
    ove::TransformProperty property,
    const JsonValue& json
)
{
    CK_ASSERT_RETURN_VALUE(json.IsArray(), ove::TransformSequence());
    
    ove::TransformSequence sequence(property, json.Size());
    
    for (auto valueIt = json.Begin();
         valueIt != json.End();
         ++valueIt) {
        
        auto& value = *valueIt;
        if (value.IsObject()) {
            ove::TransformKeyframe kf;
            kf.time = value["t"].GetDouble();
            kf.prop = value["v"].GetDouble();
            sequence.data.insertKeyframe(kf, ckm::transition::kDefault);
        }
    }
    
    return sequence;
}


} /* namespace cinek */