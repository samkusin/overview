///
/// @file Tile/TileCollectionLoader.hpp
/// Contains the class definition for the JSON based TileCollectionLoader
///
/// @author     Samir Sinha
/// @date       08/24/13
/// @copyright  Copyright 2013 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_TileCollectionLoader_hpp
#define Overview_Model_TileCollectionLoader_hpp

#include "Engine/ModelCollectionLoader.hpp"
#include "Engine/Tile/TileCollection.hpp"

#include <cinek/string.hpp>
#include <functional>

namespace cinek { namespace overview {

/// @class   TileCollectionLoader
/// @ingroup TileModel
/// @brief   Handler for unserializing TileCollection objects from an input JSON
///          stream.
///
class TileCollectionLoader : public ModelCollectionLoader
{
public:
    /// Constructor
    /// Applications supply event handlers called during unserialization
    /// @param  tileFlagConsts An object containing a bitflag enum for the
    ///                        Tile::flags member
    /// @param  atlasReqCb  Handles bitmap atlas requests made during load
    /// @param  bitmapReqCb Handles bitmap index requests made with the specific
    ///                     bitmap atlas during load
    /// @param  collectionCb Issued when a TileCollection has been unserialized
    /// @param  allocator   The allocator for memory operations
    ///
    TileCollectionLoader(
        const JsonValue& modelConsts,
        std::function<cinek_bitmap_atlas(const char*)> atlasReqCb,
        std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> bitmapReqCb,
        std::function<void(TileCollection&&)> collectionCb,
        const Allocator& allocator);

    bool startCollection(const char* name, uint32_t modelCount);
    bool parseAttribute(const char* key, const JsonValue& value);
    bool parseModel(const char* key, JsonValue& object);
    bool endCollection();

private:
    const JsonValue& _modelConsts;

    std::function<cinek_bitmap_atlas(const char*)> _atlasReqCb;
    std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> _bitmapReqCb;
    std::function<void(TileCollection&&)> _collectionCb;

    std::string _name;
    cinek_bitmap_atlas _atlasId;
    typedef std::pair<uint16_t, Tile> TilePair;
    vector< TilePair > _tiles;
};

} /* namespace overview */ } /* namespace cinek */

#endif
