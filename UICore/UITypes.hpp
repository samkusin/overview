//
//  UITypes.hpp
//  Overview
//
//  Created by Samir Sinha on 7/20/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_UI_Types_hpp
#define Overview_UI_Types_hpp

#include "CKGfx/GfxTypes.hpp"

#include "blendish_defines.h"
#include "ouitypes.h"

typedef struct NVGcontext NVGcontext;

namespace cinek { namespace uicore {

////////////////////////////////////////////////////////////////////////////////
//  Generalized Theme defines

enum
{
    UITHEME_WIDGET_HEIGHT   = BND_WIDGET_HEIGHT
};

enum
{
    UITHEME_ICON_GHOST      = BND_ICON_GHOST,
    UITHEME_ICON_NEWFOLDER  = BND_ICON_NEWFOLDER,
    UITHEME_ICON_MONKEY     = BND_ICON_MONKEY
};

enum
{
    UITHEME_CORNER_NONE     = BND_CORNER_NONE,
    UITHEME_CORNER_TOP      = BND_CORNER_TOP,
    UITHEME_CORNER_BOTTOM   = BND_CORNER_DOWN,
    UITHEME_CORNER_LEFT     = BND_CORNER_LEFT,
    UITHEME_CORNER_RIGHT    = BND_CORNER_RIGHT,
    UITHEME_CORNER_ALL      = BND_CORNER_ALL
};

////////////////////////////////////////////////////////////////////////////////
//  UI Types

enum class ListboxLayout
{
    kList,
    kGrid
};

////////////////////////////////////////////////////////////////////////////////
//  UI Subscriber class

struct FrameEvent
{
    int item;
    UIevent evtType;
};


//  Used for UI items that require data from the application (i.e. lists )
struct DataObject
{
    enum class Type
    {
        undefined,
        string,
        custom
    };
    
    enum class ImageType
    {
        undefined,
        icon,                   // themed icon
        image,                  // nvg image
        texture                 // gfx texture
    };
    
    Type type;
    ImageType imageType;
    
    
    //  data depending on object Type
    union
    {
        const char* str;        // static C String object
        void* custom;           // custom data
    }
    data;
    
    union
    {
        int iconId;
        int imageId;            // image handle
        const gfx::Texture* texture;
    }
    image;
};

class DataProvider
{
public:
    virtual ~DataProvider() {}
    
    //  Issued when the UI system needs data for rendering or lookup.  Typically
    //  used by UI items for lists or other UI items that need object data.
    virtual bool onUIDataItemRequest(int id, uint32_t row, uint32_t col, DataObject& data) {
        data.type = DataObject::Type::undefined;
        data.imageType = DataObject::ImageType::undefined;
        return false;
    }
};

class ButtonHandler
{
public:
    virtual ~ButtonHandler() {}
    
    virtual void onUIButtonHit(int id) = 0;
};

class FrameHandler
{
public:
    virtual ~FrameHandler() {}
    virtual void onUIFrameEvent(int id, const FrameEvent& evt) = 0;
};

typedef void (*RenderCallback)(void* context, NVGcontext* nvg);

} /* namespace uicore */ } /* namespace cinek */

#endif
