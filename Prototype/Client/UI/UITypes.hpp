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

namespace cinek { namespace ovproto {

////////////////////////////////////////////////////////////////////////////////
//  Generalized Theme defines

enum
{
    UITHEME_WIDGET_HEIGHT   = BND_WIDGET_HEIGHT
};

enum
{
    UITHEME_ICON_GHOST      = BND_ICON_GHOST
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
//  UI Subscriber class
class UISubscriber
{
public:
    virtual ~UISubscriber() {}
    
    virtual void onUIEvent(int evtId, int evtType) {}
};

} /* namespace ovproto */ } /* namespace cinek */

#endif
