//
//  UIEngine.hpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_UI_Engine_hpp
#define Overview_UI_Engine_hpp

#include "UITypes.hpp"

#include <cinek/cstringstack.hpp>

namespace cinek { namespace uicore {

extern void OUIHandler(int item, UIevent evt);

enum class OUIItemType
{
    frame,
    group,
    column,
    row,
    window,
    stack,
    button,
    listbox,
    combobox,
    titlebar
};


struct OUIHeader
{
    OUIItemType itemType;
    UIhandler handler;
};

struct OUIWindow
{
    OUIHeader header;
    const char* title;
};

struct OUIFrame
{
    OUIHeader header;
    FrameState* state;          /* reported back to controller */
    RenderCallback renderCb;    /* custom rendering callback */
    void* callbackContext;      /* custom rendering callback context */
    
    static void handler(int item, UIevent event);
};

struct OUIStack
{
    OUIHeader header;
    const char* title;
    bool* state;
};

struct OUIButtonData
{
    OUIHeader header;
    int id;
    ButtonHandler* fireHandler;
    int iconId;
    const char* label;
    
    static void handler(int item, UIevent event);
};

/*
 *  OUIListBox uses a UISubscriber data request to lookup what items to render 
 *  by index.
 *  
 *  Upon selection, a subscriber can lookup what item has been selected from .
 */
struct OUIListBoxData
{
    OUIHeader header;
    ListboxType lbtype;     /* layout of items */
    DataProvider* provider;
    int32_t providerId;     /* Data provider object and ID */
    UIvec2 viewAnchor;      /* Used for scrolling */
    ListboxState* state;    /* State accessible by the controller */
    
    static void handler(int item, UIevent event);
};


////////////////////////////////////////////////////////////////////////////

struct Context
{
    struct InitParams
    {
        UIcontext* ouiContext;
        size_t stringStackSize;
    };
    Context(const InitParams& params);
    
    UIcontext* ouiContext;
    Theme theme;
    int rootItem;
    int keyFocusItem;
    FrameState frameState;
    KeyEvent frameKeyEvents[16];
    CStringStack stringStack;
};


void initContext(int w, int h, Context* context);
void finalizeContext(Context* context);

int createFrame(Context* context, unsigned int eventFlags, FrameState* frameState,
    RenderCallback renderCb,
    void* cbcontext);
    

} /* namespace uicore */ } /* namespace cinek */

#endif /* defined(Overview_UI_Engine_hpp) */
