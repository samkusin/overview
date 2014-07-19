///
/// @file
/// Contains the class definition for the JSON based BlockCollectionLoader
///
/// @author     Samir Sinha
/// @date       07/16/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_BlockCollectionLoader_hpp
#define Overview_Model_BlockCollectionLoader_hpp

#include "Engine/Model/ModelCollectionLoader.hpp"
#include "Engine/Model/BlockCollection.hpp"

#include <functional>
#include <streambuf>

namespace cinekine { namespace ovengine {

#include "cinek/string.hpp"

#include <functional>

namespace cinekine { namespace ovengine {

/// @class   BlockCollectionLoader
/// @ingroup TileModel
/// @brief   Handler for unserializing BlockCollection objects from an input
///          JSON stream.
///
class BlockCollectionLoader : public ModelCollectionLoader
{
public:
    /// Constructor
    /// Applications supply event handlers called during unserialization
    /// @param  collectionCb Issued when a BlockCollection has been unserialized
    /// @param  allocator   The allocator for memory operations
    ///
    BlockCollectionLoader(std::function<void(BlockCollection&&)> collectionCb,
                          const Allocator& allocator);

    bool startCollection(const char* name, uint32_t modelCount);
    bool parseAttribute(const char* key, const JsonValue& value);
    bool parseModel(const char* key, JsonValue& object);
    bool endCollection();

private:
    std::function<void(TileCollection&&)> _collectionCb;
    string _name;
    string _tilesetName;
    BlockCollection::Container _models;
};


} /* namespace ovengine */ } /* namespace cinekine */

#endif
