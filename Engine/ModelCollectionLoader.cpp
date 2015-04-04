//
//  ModelCollectionLoader.cpp
//  Implementation
//
//  Copyright 2014 Cinekine Media All rights reserved
//  Author: Samir Sinha
//  License: The MIT License (MIT)

#include "Engine/ModelCollectionLoader.hpp"
#include "Engine/Debug.hpp"

#include <cinek/json/jsonstreambuf.hpp>
#include <cinek/json/json.hpp>
#include <cstring>

namespace cinek { namespace overview {

//
// The Unserializer object must implement the following methods:
// bool startCollection(const char* name);
// bool parseAttribute(const char* key, const JsonValue& value);
// bool parseModel(const char* key, JsonValue& object);
// bool endCollection();
bool unserializeFromJSON(std::streambuf& instream, ModelCollectionLoader& handler)
{
    RapidJsonStdStreamBuf jsonStream(instream);

    JsonDocument jsonDoc;
    jsonDoc.ParseStream<0>(jsonStream);

    if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
    {
        OVENGINE_LOG_ERROR("unserializeFromJSON<> - stream is not valid JSON.");
        return false;
    }

    //  iterate through all collections
    for (auto docIter = jsonDoc.MemberBegin(),
              docIterEnd = jsonDoc.MemberEnd();
         docIter != docIterEnd;
         ++docIter)
    {
        //  iterate through all attributes in this collection
        auto& collection = *docIter;
        const char* collectionName = collection.name.GetString();
        if (!collection.value.IsObject())
        {
            OVENGINE_LOG_ERROR("unserializeFromJSON<> - collection %s is not "
                               "a valid JSON object.",
                               collectionName);
            return false;
        }

        auto& items = collection.value["collection"];

        if (!handler.startCollection(collectionName, (int)(items.MemberEnd() - items.MemberBegin())))
        {
            OVENGINE_LOG_ERROR("unserializeFromJSON<> - startCollection(%s) failed",
                               collectionName);
            return false;
        }
        //  parse the collection
        for (auto colIter = collection.value.MemberBegin(),
                  colIterEnd = collection.value.MemberEnd();
             colIter != colIterEnd;
             ++colIter)
        {
            auto& attribute = *colIter;
            const char* attrKey = attribute.name.GetString();
            if (!strcmp(attrKey, "collection")) // collections are later
                continue;
            if (!handler.parseAttribute(attrKey, attribute.value))
            {
                OVENGINE_LOG_ERROR("unserializeFromJSON<> - %s.parseAttribute(%s) failed",
                                   collectionName, attrKey);
                return false;
            }
        }

        if (!items.IsNull())
        {
            //  iterate through all models
            for (auto modelIter = items.MemberBegin(),
                      modelIterEnd = items.MemberEnd();
                 modelIter != modelIterEnd;
                 ++modelIter)
            {
                auto& model = *modelIter;
                if (!handler.parseModel(model.name.GetString(), model.value))
                {
                    OVENGINE_LOG_ERROR("unserializeFromJSON<> - %s.parseModel(%s) failed",
                                       collectionName, model.name.GetString());
                }
            }
        }
        if (!handler.endCollection())
        {
            OVENGINE_LOG_ERROR("unserializeFromJSON<> - %s.endCollection() failed",
                               collectionName);
            return false;
        }
    }
    return true;
}

} /* namespace overview */ } /* namespace cinek */
