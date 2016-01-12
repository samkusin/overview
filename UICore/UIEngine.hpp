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

#include <cinek/vector.hpp>

namespace cinek { namespace uicore {

extern void OUIHandler(int item, UIevent evt);

enum class OUIItemType
{
    frame,
    column,
    window,
    button,
    listbox,
    combobox
};

struct OUIHeader
{
    OUIItemType itemType;
    UIhandler handler;
};

struct OUIFrame
{
    OUIHeader header;
    int id;
    FrameHandler* frameHandler;
    RenderCallback renderCb;
    void* callbackContext;
    
    static void handler(int item, UIevent event);
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
    int32_t *selected;      /* what data object is active (selected) */
    int32_t hover;          /* what data object is in the hover state */
    
    static void handler(int item, UIevent event);
    
};

} /* namespace uicore */ } /* namespace cinek */

#endif /* defined(Overview_UI_Engine_hpp) */
