//
//  TileDatabaseLoader.h
//  Overview
//
//  Created by Samir Sinha on 8/24/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_TileDatabaseLoader_hpp
#define Overview_TileDatabaseLoader_hpp

#include "BitmapDatabaseLoader.hpp"


namespace cinekine {
    namespace rendermodel {
        class TileDatabase;
    }
}

namespace cinekine {
    namespace ovengine {

    /**
     * @class TileDatabaseLoader
     * @brief Handles unserialization of a TileDatabase
     */
    class TileDatabaseLoader : public BitmapDatabaseLoader
    {
    public:
        /**
         * @param database The source/target database.
         */
        TileDatabaseLoader(rendermodel::TileDatabase& database);

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
        rendermodel::TileDatabase& _db;
    };

    }   // namespace ovengine
}   // namespace cinekine


#endif