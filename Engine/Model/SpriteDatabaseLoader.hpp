/**
 * @file    Model/SpriteDatabaseLoader.hpp
 *
 * Handlers  unserialization of sprite database configuration into
 * a SpriteDatabase.
 *
 * @note    Created by Samir Sinha on 7/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_SpriteDatabaseLoader_hpp
#define Overview_SpriteDatabaseLoader_hpp

#include "cinek/rendermodel/types.h"

#include <functional>
#include <streambuf>

namespace cinekine {
    namespace rendermodel {
        class SpriteDatabase;
    }
    namespace ovengine {
    
    /**
     * @class SpriteDatabaseLoader
     * @brief Handles unserialization of a SpriteDatabase 
     */
    class SpriteDatabaseLoader
    {
    public:
        /**
         * @param database The source/target database.
         */
        SpriteDatabaseLoader(rendermodel::SpriteDatabase& database);

        /**
         * Unserializes a database from the given document object.  Serialization
         * events are dispatched via callbacks supplied by the caller via the delegate
         * methods.
         * @param instream              A JSON formatted stream.
         * @return                      False if there is a parsing error.
         */
        bool unserialize(std::streambuf& instream);

        /** @param requestCb    Delegate method, function issued to request a bitmap 
            atlas from a renderer. */
        void onBitmapAtlasRequest(std::function<cinek_bitmap_atlas(const char*)>& requestCb) {
            _atlasRequest = requestCb;
        }
        /** @param requestCb    Delegate method, function issued to request a bitmap
            index from a renderer. */
        void onBitmapIndexRequest(std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)>& requestCb) {
            _indexRequest = requestCb;
        }

    private:
        rendermodel::SpriteDatabase& _db;
        
        std::function<cinek_bitmap_atlas(const char*)> _atlasRequest;
        std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> _indexRequest;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif
