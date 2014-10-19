///
/// @file
/// Contains the class definition for the SpriteLibrary
///
/// @author     Samir Sinha
/// @date       07/05/13
/// @copyright  Copyright 2013 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_SpriteLibraryLoader_hpp
#define Overview_Model_SpriteLibraryLoader_hpp

#include "cinek/rendertypes.h"

#include <functional>
#include <streambuf>

namespace cinek { namespace overview {

class SpriteLibrary;

/// Unserializes a TileCollection from a JSON formatted input stream.
/// @ingroup    SpriteModel
/// @param[out] outLibrary The output SpriteLibrary
/// @param      instream   The input stream
/// @param      atlasReqCb Request delegate for a Bitmap Atlas index from a name
///                        during unserialization
/// @param      bitmapReqCb Request delegate for a bitmap index from the bitmap
///                         name during unserialization
/// @return False on error
///
bool unserializeFromJSON(SpriteLibrary& outLibrary,
            std::streambuf& instream,
            std::function<cinek_bitmap_atlas(const char*)> atlasReqCb,
            std::function<cinek_bitmap_index(cinek_bitmap_atlas, const char*)> bitmapReqCb);

    }   // namespace overview
}   // namespace cinek

#endif
