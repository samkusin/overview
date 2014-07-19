///
/// @file
/// The ModelCollectionLoader common class header
///
/// @author     Samir Sinha
/// @date       07/17/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_CollectionLoader_hpp
#define Overview_Model_CollectionLoader_hpp

#include "Core/JsonUtilities.hpp"

#include <streambuf>

namespace cinekine { namespace ovengine {

/// @class   ModelCollectionLoader
/// @ingroup TileModel
/// @brief   Applications implement ModelCollectionLoader to unserializer
///          a group of ModelCollection objects
///
/// When any of the methods return false, parsing aborts.
///
class ModelCollectionLoader
{
public:
    virtual ~ModelCollectionLoader() {}
    virtual bool startCollection(const char* name, uint32_t modelCount) = 0;
    virtual bool parseAttribute(const char* key, const JsonValue& value) = 0;
    virtual bool parseModel(const char* key, JsonValue& object) = 0;
    virtual bool endCollection() = 0;
};

/// Unserializes a Model Collection from source JSON.
/// The input JSON must be an object containing one or more collections.
/// Applications supply an Unserializer specialized for a particular
/// ModelCollection type.
///
/// @ingroup TileModel
///
/// @param  instream The input stream
/// @param  handler  The Unserializer object that implements the
///                  ModelCollectionLoader interface.
/// @return False if parsing fails
///
bool unserializeFromJSON(std::streambuf& instream, ModelCollectionLoader& handler);

} /* namespace ovengine */ } /* namespace cinekine */

#endif
