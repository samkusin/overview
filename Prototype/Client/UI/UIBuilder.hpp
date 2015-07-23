//
//  UIBuilder.hpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_UI_Builder_hpp
#define Overview_UI_Builder_hpp

#include "UITypes.hpp"

#include <cinek/vector.hpp>

namespace cinek { namespace ovproto {


/*
    layout.frame(layout_flags).events(UI_BUTTON0_DOWN).size(w,h).
        end();
        
    layout.frame().events(UI_BUTTON0_DOWN).size(w,h).
        column(UI_TOP|UI_HFILL).margins(10,10,10,10).
            button(BND_ICON_GHOST, "Item 1", ui_subscriber, ID_OK).
            hgroup().margins(5,5,5,5).
                radio(BND_ICON_REC, "Radio 1", &radio1).
                radio(BND_ICON_REC, "Radio 2", &radio1).
                radio(BND_ICON_REC, "Radio 3", &radio1).
            end().
        end().
        row().
            ...
        end().
    end();
 
*/

class UILayout
{
public:
    UILayout();
    
    ~UILayout();
    
    ///
    /// Starts a frame - which is an encompassing region designed to act as an
    /// Event subscriber for views, or as a container for other ui items.
    ///
    UILayout& frame(uint32_t layoutFlags=0);
    ///
    /// Starts a vertical group of ui widgets - used for positioning.
    ///
    UILayout& column(uint32_t layoutFlags=0);
    ///
    /// Subscribe the current UI group to one or more events.
    ///
    UILayout& events(uint32_t flags, UISubscriber* subscriber, int32_t evtId);
    ///
    /// Sets the size of the current UI region.
    ///
    UILayout& size(int32_t w, int32_t h);
    ///
    /// An individual button widget within the encompassing UI group.
    ///
    UILayout& button(int32_t iconId, const char* label,
                     UISubscriber* subscriber, int32_t evtId);
    
    UILayout& end();
    
private:
    static constexpr size_t kDataSize = sizeof(vector<int32_t>);
    static constexpr size_t kMaxItemsInData = kDataSize / sizeof(int32_t);
    
    uint8_t _data[kDataSize];
    uint32_t _size;             // high bit, then _data contains a vector
    int32_t _topItem;
    
    enum
    {
        kFreeLayout,
        kColumnLayout
    }
    _mode;
    
    void insertItem(int32_t item, int32_t parent);
    void pushTop();
    int32_t popItem();
};


} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(UI_Builder) */
