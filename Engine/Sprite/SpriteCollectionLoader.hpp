///
/// @file Sprite/SpriteCollectionLoader.hpp
/// Contains the class definition for the SpriteCollection
///
/// @author     Samir Sinha
/// @date       07/05/13
/// @copyright  Copyright 2013 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_SpriteCollectionLoader_hpp
#define Overview_Model_SpriteCollectionLoader_hpp

#include "Engine/Sprite/Sprite.hpp"

#include <cinek/json/jsoncollectionloader.hpp>
#include <cinek/allocator.hpp>
#include <cinek/vector.hpp>
#include <functional>
#include <streambuf>

namespace cinek {
    namespace gfx {
        class BitmapLibrary;
    }
}

namespace cinek { namespace overview {

class SpriteCollection;

class SpriteCollectionLoader : public JsonCollectionLoader
{
public:
    using RequestAtlasCb = std::function<gfx::BitmapAtlasHandle(const char* atlasName)>;
    using CollectionLoadedCb = std::function<void(SpriteCollection&& )>;
    

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
    SpriteCollectionLoader
    (
        const gfx::BitmapLibrary& bitmapLibrary,
        const JsonValue& spriteConsts,
        const RequestAtlasCb& reqAtlasCb,
        const CollectionLoadedCb& collectionLoadedCb,
        const Allocator& allocator
    );

    bool startCollection(const char* name, uint32_t modelCount);
    bool parseAttribute(const char* key, const JsonValue& value);
    bool parseModel(const char* key, JsonValue& object);
    bool endCollection();

private:
    const gfx::BitmapLibrary* _bitmapLibrary;
    const JsonValue* _spriteConsts;

    RequestAtlasCb _reqAtlasCb;
    CollectionLoadedCb _collectionLoadedCb;

    std::string _name;
    vector< Sprite > _sprites;
};

    }   // namespace overview
}   // namespace cinek

#endif
