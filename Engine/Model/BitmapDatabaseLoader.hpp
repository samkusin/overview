//
//  Model/BitmapDatabaseLoader.hpp
//  Overview
//
//  Created by Samir Sinha on 8/24/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_BitmapDatabaseLoader_hpp
#define Overview_BitmapDatabaseLoader_hpp

#include "cinek/rendertypes.h"

#include <functional>
#include <streambuf>

namespace cinekine {
    namespace ovengine {

    /**
     * @class BitmapDatabaseLoader
     * @brief Handles unserialization of a BitmapDatabase, reporting new bitmaps
     *        to listeners.
     */
    class BitmapDatabaseLoader
    {
    public:
        virtual ~BitmapDatabaseLoader() {}

        /**
         * Unserializes a database from the given document object.  Serialization
         * events are dispatched via callbacks supplied by the caller via the delegate
         * methods.
         * @param instream              A JSON formatted stream.
         * @return                      False if there is a parsing error.
         */
        virtual bool unserialize(std::streambuf& instream,
                                 std::function<cinek_bitmap_atlas(const char*)> atlasReqCb,
                                 std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> bitmapReqCb) = 0;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif