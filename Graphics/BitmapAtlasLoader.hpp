//
//  BitmapAtlasSerializer.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CK_Graphics_BitmapAtlasLoader_hpp
#define CK_Graphics_BitmapAtlasLoader_hpp

#include "BitmapAtlas.hpp"

#include <functional>
#include <streambuf>

namespace cinek {
    namespace glx {

    /**
     * @class BitmapAtlasLoader
     * @brief Handles unserialization of a BitmapAtlas
     */
    class BitmapAtlasLoader
    {
    public:
        /**
         * @param database The source/target database.
         */
        BitmapAtlasLoader() = default;

        /**
         * Unserializes a database from the given document object.  Serialization
         * events are dispatched via callbacks supplied by the caller via the delegate
         * methods.
         * @param instream              A JSON formatted stream.
         * @return                      False if there is a parsing error.
         */
        bool unserialize(std::streambuf& instream);

        /**
         * Unserialize an image filename from the stream, passing it to the supplied delegate.
         * Passed to the delegate are: (filename, width and height and format.)
         * @param requestCb    Delegate method issued when loading an image.
         */
        void onImageLoadRequest(std::function<bool(const char*, uint16_t, uint16_t, cinek_pixel_format, size_t)> requestCb) {
            _loadImageRequest = requestCb;
        }
        /** @param requestCb    Delegate method issued when registering a bitmap frame */
        void onNewFrameRequest(std::function<void(BitmapInfo& info)> requestCb) {
            _newFrameRequest = requestCb;
        }

    private:
        std::function<bool(const char*, uint16_t, uint16_t, cinek_pixel_format, size_t)> _loadImageRequest;
        std::function<void(BitmapInfo& info)> _newFrameRequest;
    };

    }   // namespace glx
}   // namespace cinek

#endif
