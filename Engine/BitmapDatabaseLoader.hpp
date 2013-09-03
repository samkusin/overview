//
//  BitmapDatabaseLoader.hpp
//  Overview
//
//  Created by Samir Sinha on 8/24/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_BitmapDatabaseLoader_hpp
#define Overview_BitmapDatabaseLoader_hpp

#include "cinek/rendermodel/types.h"

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
            BitmapDatabaseLoader() = default;
            
            /**
             * Unserializes a database from the given document object.  Serialization
             * events are dispatched via callbacks supplied by the caller via the delegate
             * methods.
             * @param instream              A JSON formatted stream.
             * @return                      False if there is a parsing error.
             */
            virtual bool unserialize(std::streambuf& instream) = 0;
            
            /** @param requestCb    Delegate method, function issued to request a bitmap
             atlas from a renderer. */
            void onBitmapAtlasRequest(std::function<cinek_bitmap_atlas(const char*)> requestCb) {
                _atlasRequest = requestCb;
            }
            /** @param requestCb    Delegate method, function issued to request a bitmap
             index from a renderer. */
            void onBitmapIndexRequest(std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> requestCb) {
                _indexRequest = requestCb;
            }
            
        protected:
            std::function<cinek_bitmap_atlas(const char*)> _atlasRequest;
            std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> _indexRequest;
        };
        
    }   // namespace ovengine
}   // namespace cinekine

#endif