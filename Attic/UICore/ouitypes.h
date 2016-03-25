/*
OUI - A minimal semi-immediate GUI handling & layouting library

Copyright (c) 2014 Leonard Ritter <leonard.ritter@duangle.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#ifndef _OUI_TYPES_H_
#define _OUI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*

Revision 5 (2015-7-21) [Samir Sinha]

Moved Types into its own header so that other headers/source that only reference
OUI types but have no need to invoke its functions.

Revision 4 (2014-12-17)

OUI (short for "Open UI", spoken like the french "oui" for "yes") is a
platform agnostic single-header C library for layouting GUI elements and
handling related user input. Together with a set of widget drawing and logic 
routines it can be used to build complex user interfaces.

OUI is a semi-immediate GUI. Widget declarations are persistent for the duration
of the setup and evaluation, but do not need to be kept around longer than one
frame.

OUI has no widget types; instead, it provides only one kind of element, "Items",
which can be tailored to the application by the user and expanded with custom
buffers and event handlers to behave as containers, buttons, sliders, radio
buttons, and so on.

OUI also does not draw anything; Instead it provides a set of functions to
iterate and query the layouted items in order to allow client code to render
each widget with its current state using a preferred graphics library.

See example.cpp in the repository for a full usage example.
*/

// you can override this from the outside to pick
// the export level you need
#ifndef OUI_EXPORT
#define OUI_EXPORT
#endif

// some language bindings (e.g. terra) have no good support
// for unions or unnamed structs; 
// #define OUI_USE_UNION_VECTORS 0 to disable.
#ifndef OUI_USE_UNION_VECTORS
#define OUI_USE_UNION_VECTORS 1
#endif

// limits

enum {
    // maximum size in bytes of a single data buffer passed to uiAllocData().
    UI_MAX_DATASIZE = 4096,
    // maximum depth of nested containers
    UI_MAX_DEPTH = 64,
    // maximum number of buffered input events
    UI_MAX_INPUT_EVENTS = 64,
    // consecutive click threshold in ms
    UI_CLICK_THRESHOLD = 250,
};

typedef unsigned int UIuint;

// opaque UI context
typedef struct UIcontext UIcontext;

// item states as returned by uiGetState()

typedef enum UIitemState {
    // the item is inactive
    UI_COLD = 0,
    // the item is inactive, but the cursor is hovering over this item
    UI_HOT = 1,
    // the item is toggled, activated, focused (depends on item kind)
    UI_ACTIVE = 2,
    // the item is unresponsive
    UI_FROZEN = 3,
} UIitemState;

// container flags to pass to uiSetBox()
typedef enum UIboxFlags {
    // flex-direction (bit 0+1)

    // left to right
    UI_ROW = 0x002,
    // top to bottom
    UI_COLUMN = 0x003,

    // model (bit 1)

    // free layout
    UI_LAYOUT = 0x000,
    // flex model
    UI_FLEX = 0x002,

    // flex-wrap (bit 2)

    // single-line
    UI_NOWRAP = 0x000,
    // multi-line, wrap left to right
    UI_WRAP = 0x004,


    // justify-content (start, end, center, space-between)
    // at start of row/column
    UI_START = 0x008,
    // at center of row/column
    UI_MIDDLE = 0x000,
    // at end of row/column
    UI_END = 0x010,
    // insert spacing to stretch across whole row/column
    UI_JUSTIFY = 0x018,

    // align-items
    // can be implemented by putting a flex container in a layout container,
    // then using UI_TOP, UI_DOWN, UI_VFILL, UI_VCENTER, etc.
    // FILL is equivalent to stretch/grow

    // align-content (start, end, center, stretch)
    // can be implemented by putting a flex container in a layout container,
    // then using UI_TOP, UI_DOWN, UI_VFILL, UI_VCENTER, etc.
    // FILL is equivalent to stretch; space-between is not supported.
} UIboxFlags;

// child layout flags to pass to uiSetLayout()
typedef enum UIlayoutFlags {
    // attachments (bit 5-8)
    // fully valid when parent uses UI_LAYOUT model
    // partially valid when in UI_FLEX model

    // anchor to left item or left side of parent
    UI_LEFT = 0x020,
    // anchor to top item or top side of parent
    UI_TOP = 0x040,
    // anchor to right item or right side of parent
    UI_RIGHT = 0x080,
    // anchor to bottom item or bottom side of parent
    UI_DOWN = 0x100,
    // anchor to both left and right item or parent borders
    UI_HFILL = 0x0a0,
    // anchor to both top and bottom item or parent borders
    UI_VFILL = 0x140,
    // center horizontally, with left margin as offset
    UI_HCENTER = 0x000,
    // center vertically, with top margin as offset
    UI_VCENTER = 0x000,
    // center in both directions, with left/top margin as offset
    UI_CENTER = 0x000,
    // anchor to all four directions
    UI_FILL = 0x1e0,
    // when wrapping, put this element on a new line
    // wrapping layout code auto-inserts UI_BREAK flags,
    // drawing routines can read them with uiGetLayout()
    UI_BREAK = 0x200
} UIlayoutFlags;

// event flags
typedef enum UIevent {
    // added null event
    UI_EVENT_NULL = 0x0000,
    // on button 0 down
    UI_BUTTON0_DOWN = 0x0400,
    // on button 0 up
    // when this event has a handler, uiGetState() will return UI_ACTIVE as
    // long as button 0 is down.
    UI_BUTTON0_UP = 0x0800,
    // on button 0 up while item is hovered
    // when this event has a handler, uiGetState() will return UI_ACTIVE
    // when the cursor is hovering the items rectangle; this is the
    // behavior expected for buttons.
    UI_BUTTON0_HOT_UP = 0x1000,
    // item is being captured (button 0 constantly pressed); 
    // when this event has a handler, uiGetState() will return UI_ACTIVE as
    // long as button 0 is down.
    UI_BUTTON0_CAPTURE = 0x2000,
    // on button 2 down (right mouse button, usually triggers context menu)
    UI_BUTTON2_DOWN = 0x4000,
    // item has received a scrollwheel event
    // the accumulated wheel offset can be queried with uiGetScroll()
    UI_SCROLL = 0x8000,
    // item is focused and has received a key-down event
    // the respective key can be queried using uiGetKey() and uiGetModifier()
    UI_KEY_DOWN = 0x10000,
    // item is focused and has received a key-up event
    // the respective key can be queried using uiGetKey() and uiGetModifier()
    UI_KEY_UP = 0x20000,
    // item is focused and has received a character event
    // the respective character can be queried using uiGetKey()
    UI_CHAR = 0x40000,
} UIevent;

enum {
    // these bits, starting at bit 24, can be safely assigned by the
    // application, e.g. as item types, other event types, drop targets, etc.
    // they can be set and queried using uiSetFlags() and uiGetFlags()
    UI_USERMASK = 0xff000000,

    // a special mask passed to uiFindItem()
    UI_ANY = 0xffffffff,
};

// handler callback; event is one of UI_EVENT_*
typedef void (*UIhandler)(int item, UIevent event);

// for cursor positions, mainly
typedef struct UIvec2 {
#if OUI_USE_UNION_VECTORS || defined(OUI_IMPLEMENTATION)
    union {
        int v[2];
        struct { int x, y; };
    };
#else
    int x, y;
#endif    
} UIvec2;

// layout rectangle
typedef struct UIrect {
#if OUI_USE_UNION_VECTORS || defined(OUI_IMPLEMENTATION)    
    union {
        int v[4];
        struct { int x, y, w, h; };
    };
#else
    int x, y, w, h;
#endif
} UIrect;

#ifdef __cplusplus
};
#endif

#endif
