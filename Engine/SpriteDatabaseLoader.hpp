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

#include "BitmapDatabaseLoader.hpp"

namespace cinekine {
    namespace rendermodel {
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
        SpriteDatabaseLoader(rendermodel::SpriteDatabase& database);

        /**
         * Unserializes a database from the given document object.  Serialization
         * events are dispatched via callbacks supplied by the caller via the delegate
         * methods.
         * @param instream              A JSON formatted stream.
         * @return                      False if there is a parsing error.
         */
        bool unserialize(std::streambuf& instream);

    private:
        rendermodel::SpriteDatabase& _db;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif
