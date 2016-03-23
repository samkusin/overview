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

namespace cinek {

namespace uicore {

struct System;
struct InputState;
struct Context;
struct Theme;

class Layout;

////////////////////////////////////////////////////////////////////////////////
//  Generalized Theme defines

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

enum class ListboxType
{
    kList
};

struct Box
{
    int t, r, b, l;
};

struct Color
{
    float r,g,b,a;
    
    Color& fromRGBA8(unsigned char r_,
                     unsigned char g_,
                     unsigned char b_,
                     unsigned char a_)
    {
        r = r_/255.0f;
        g = g_/255.0f;
        b = b_/255.0f;
        a = a_/255.0f;
        
        return *this;
    }
};

struct KeyEvent
{
    UIevent type;
    unsigned int mod;
    unsigned int key;
    
    explicit operator bool() const {
        return type != UI_EVENT_NULL;
    }
};

////////////////////////////////////////////////////////////////////////////////
//  UI State Data (for imgui-like query)

struct ListboxState
{
    int thisItem;
    int highlightItem;          // Item currently highlighted
    int hoverItem;              // Item in the hover state (reset when laying out)
    int selectedItem;           // Item selected (reset when laying out)
    
    void init(int item) {
        thisItem = item;
        hoverItem = -1;
        selectedItem = -1;
    }
    
    bool selected() const {
        return selectedItem != -1 && highlightItem == selectedItem;
    }
};

struct FrameState
{
    int thisItem;
    UIevent evtType;
    
    KeyEvent* keyEvents;
    int keyEventStart;
    int keyEventCount;
    int keyEventLimit;
    
    FrameState() : keyEvents(nullptr), keyEventLimit(0) {}
    
    void init(int item) {
        evtType = UI_EVENT_NULL;
        thisItem = item;
        keyEventStart = 0;
        keyEventCount = 0;
    }
    
    KeyEvent popKey() {
        if (keyEventStart < keyEventCount) {
            return keyEvents[keyEventStart++];
        }
        return { UI_EVENT_NULL };
    }
};


////////////////////////////////////////////////////////////////////////////////
//  UI Subscriber class


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
        image                   // nvg image
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
    }
    image;
    
    DataObject() : type(Type::undefined), imageType(ImageType::undefined) {}
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
    //  Issued when a UI needs to retrieve an item count for the specified data
    //  index
    virtual uint32_t onUIDataItemRowCountRequest(int id) {
        return 0;
    }
};

class ButtonHandler
{
public:
    virtual ~ButtonHandler() {}
    
    virtual void onUIButtonHit(int id) = 0;
};

typedef void (*RenderCallback)(void* context, NVGcontext* nvg);

} /* namespace uicore */ } /* namespace cinek */


#endif
