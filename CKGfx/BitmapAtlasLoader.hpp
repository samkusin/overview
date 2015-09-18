//
//  BitmapAtlasSerializer.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_BitmapAtlasLoader_hpp
#define CK_Graphics_BitmapAtlasLoader_hpp

#include "CKGfx/BitmapAtlas.hpp"

#include <ckjson/jsoncollectionloader.hpp>
#include <functional>
#include <streambuf>

namespace cinek {
    namespace gfx {

/**
 * @class BitmapAtlasLoader
 * @brief Handles unserialization of a BitmapAtlas
 */
class BitmapAtlasLoader : public JsonCollectionLoader
{
public:
    using LoadTextureCb = std::function<TextureHandle(const char* texPath)>;
    using AtlasLoadedCb = std::function<void(unique_ptr<BitmapAtlas>&& atlas)>;
    
    /// Constructor
    /// Applications supply event handlers called during unserialization
    /// @param  atlasLoadedCb   Returns a populated BitmapAtlas
    /// @param  allocator   The allocator for memory operations
    ///
    BitmapAtlasLoader
    (
        const LoadTextureCb& loadTextureCb,
        const AtlasLoadedCb& atlasLoadedCb,
        const Allocator& allocator = Allocator()
    );
   
    bool startCollection(const char* name, uint32_t modelCount);
    bool parseAttribute(const char* key, const JsonValue& value);
    bool parseModel(const char* key, JsonValue& object);
    bool endCollection();
   
private:
    Allocator _allocator;
    LoadTextureCb _loadTextureCb;
    AtlasLoadedCb _atlasLoadedCb;
    std::string _imgPath;
    unique_ptr<BitmapAtlas> _atlas;
};

    }   // namespace glx
}   // namespace cinek

#endif
