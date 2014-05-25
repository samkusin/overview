/**
 * @file    Model/SpriteDatabaseLoader.hpp
 *
 * Handlers  unserialization of sprite database configuration into
 * a SpriteDatabase.
 *
 * @note    Created by Samir Sinha on 7/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Model_SpriteDatabaseLoader_hpp
#define Overview_Model_SpriteDatabaseLoader_hpp

#include "Engine/Model/BitmapDatabaseLoader.hpp"

namespace cinekine {
    namespace ovengine {
        class SpriteDatabase;
    }
    namespace ovengine {

    /**
     * @class SpriteDatabaseLoader
     * @brief Handles unserialization of a SpriteDatabase
     */
    class SpriteDatabaseLoader : public BitmapDatabaseLoader
    {
    public:
        /**
         * @param database The source/target database.
         */
        SpriteDatabaseLoader(SpriteDatabase& database);

        /**
         * Unserializes a database from the given document object.
         * Serialization events are dispatched via callbacks supplied by the
         * caller.
         * @param   instream    A JSON formatted stream.
         * @param   atlasReqCb  Callback issued when a new atlas was discovered
         * @param   bitmapReqCb Callback issued when unserialzing a bitmap
         * @return  False if there is a parsing error.
         */
        bool unserialize(std::streambuf& instream,
                    std::function<cinek_bitmap_atlas(const char*)> atlasReqCb,
                    std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> bitmapReqCb);

    private:
        SpriteDatabase& _db;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif
