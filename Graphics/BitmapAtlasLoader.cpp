//
//  BitmapAtlasLoader.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "BitmapAtlasLoader.hpp"

#include "RenderDebug.hpp"
#include "Framework/StreamBufRapidJson.hpp"

#include "rapidjson/document.h"

#include <cstring>

namespace cinekine {
    namespace glx {

    bool BitmapAtlasLoader::unserialize(std::streambuf& instream)
    {
        typedef rapidjson::GenericDocument<rapidjson::UTF8<> > Document;
        typedef rapidjson::GenericValue<rapidjson::UTF8<> > Value;

        RapidJsonStdStreamBuf jsonStream(instream);

        Document jsonDoc;
        jsonDoc.ParseStream<0>(jsonStream);

        if (jsonDoc.HasParseError() || !jsonDoc.IsObject())
        {
            RENDER_LOG_ERROR("BitmapAtlasLoader - failed to parse.");
            return false;
        }

        size_t frameCount = (jsonDoc.HasMember("frames") && jsonDoc["frames"].IsArray()) ? jsonDoc["frames"].Size() : 16;

        //  parse metadata
        if (jsonDoc.HasMember("meta") && jsonDoc["meta"].IsObject())
        {
            const Value& meta = jsonDoc["meta"];
            if (!meta.HasMember("image") || !meta.HasMember("size") || !meta.HasMember("format"))
            {
                RENDER_LOG_ERROR("BitmapAtlasLoader - no image information found.");
                return false;
            }
            cinek_pixel_format pixelFormat = kCinekPixelFormat_Unknown;
            const char* format = meta["format"].GetString();
            if (!strcmp(format, "RGB888"))
            {
                pixelFormat = kCinekPixelFormat_RGB888;
            }
            else if (!strcmp(format, "RGBA8888"))
            {
                pixelFormat = kCinekPixelFormat_ARGB8888;
            }
            else if (!strcmp(format, "RGBA4444"))
            {
                pixelFormat = kCinekPixelFormat_ARGB4444;
            }
            else if (!strcmp(format, "RGBA5551"))
            {
                pixelFormat = kCinekPixelFormat_ARGB1555;
            }
            else if (!strcmp(format, "RGB565"))
            {
                pixelFormat = kCinekPixelFormat_RGB565;
            }
            const Value& size = meta["size"];
            const char* imageName = meta["image"].GetString();
            if (!_loadImageRequest(imageName, size["w"].GetUint(), size["h"].GetUint(), pixelFormat, frameCount))
            {
                RENDER_LOG_ERROR("BitmapAtlasLoader - failed to load image '%s'", imageName);
                return false;
            }
        }

        //  add or append to the states table.
        if (frameCount)
        {
            const Value& frames = jsonDoc["frames"];

            for (Value::ConstValueIterator it = frames.Begin(), itEnd = frames.End();
                 it != itEnd;
                 ++it)
            {
                const Value& frame = *it;
                if (!frame.HasMember("filename") || !frame.HasMember("frame"))
                {
                    RENDER_LOG_WARN("BitmapAtlasLoader - Invalid frame definition.  Skipping...");
                    continue;
                }
                const Value& frameRect = frame["frame"];
                const Value& sourceSize = frame["sourceSize"];
                const Value& spriteSourceSize = frame["spriteSourceSize"];
                BitmapInfo info;
                info.x = frameRect["x"].GetUint();
                info.y = frameRect["y"].GetUint();
                info.w = frameRect["w"].GetUint();
                info.h = frameRect["h"].GetUint();
                info.offX = spriteSourceSize["x"].GetUint();
                info.offY = spriteSourceSize["y"].GetUint();
                info.srcW = sourceSize["w"].GetUint();
                info.srcH = sourceSize["h"].GetUint();
                info.name = frame["filename"].GetString();
                _newFrameRequest(info);
            }
        }

        return true;
    }


    }   // namespace glx
}   // namespace cinekine
