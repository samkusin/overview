///
/// @file Entity/EntityTemplateCollectionLoader.hpp
/// Contains the class definition for the JSON based
/// EntityTemplateCollectionLoader
///
/// @author     Samir Sinha
/// @date       10/18/14
/// @copyright  Copyright 2013 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Game_Model_EntityTemplateCollectionLoader_hpp
#define Overview_Game_Model_EntityTemplateCollectionLoader_hpp

#include "Engine/Entity/EntityTemplateCollection.hpp"
#include "Engine/ModelCollectionLoader.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace overview {

/// @class   EntityTemplateCollectionLoader
/// @brief   Handler for unserializing EntityTemplateCollection objects
///          from an input JSON stream.
///
class EntityTemplateCollectionLoader : public ModelCollectionLoader
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
    EntityTemplateCollectionLoader(
        const JsonValue& tileFlagConsts,
        std::function<void(EntityTemplateCollection&&)> collectionCb,
        const Allocator& allocator);

    bool startCollection(const char* name, uint32_t modelCount);
    bool parseAttribute(const char* key, const JsonValue& value);
    bool parseModel(const char* key, JsonValue& object);
    bool endCollection();

private:
    const JsonValue& _tileFlagConsts;
    std::function<void(EntityTemplateCollection&&)> _collectionCb;
    std::string _name;
    vector<EntityTemplate> _templates;
};

} /* namespace overview */ } /* namespace cinek */

#endif
