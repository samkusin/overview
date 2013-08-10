/**
 * @file    common/modelserializer.hpp
 *
 * Container for SpriteTemplates.
 *
 * @note    Created by Samir Sinha on 7/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_modelserializer_hpp
#define Overview_modelserializer_hpp

#include "cinek/rendermodel/types.h"

#include <functional>
#include <streambuf>

namespace cinekine {
    namespace rendermodel {
        class SpriteDatabase;
    }
    namespace ovengine {

/**
 * @class SpriteDatabaseJSONSerializer
 * @brief Handles unserialization of a SpriteDatabase 
 */
class SpriteDatabaseJSONSerializer
{
public:
    /**
     * @param database The source/target database.
     */
    SpriteDatabaseJSONSerializer(rendermodel::SpriteDatabase& database);

    /**
     * Unserializes a database from the given document object.
     * @param instream              A JSON formatted stream.
     * @param getBitmapClassCb      Issued to request a bitmap class from a
     *                              renderer.
     * @param getBitmapIndexCb      Issued to request a bitmap index from a
     *                              renderer.
     * @return                      False if there is a parsing error.
     */
    bool unserialize(std::streambuf& instream,
        std::function<cinek_bitmap_class(const char*)>& getBitmapClassCb,
        std::function<cinek_bitmap_index(cinek_bitmap_class, const char*)>& getBitmapIndexCb);

private:
    rendermodel::SpriteDatabase& _db;
};

    }   // namespace ovengine
}   // namespace cinekine

#endif
