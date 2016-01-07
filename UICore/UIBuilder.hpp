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

#include <functional>

namespace cinek { namespace uicore {


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

class Layout
{
public:
    Layout();
    ~Layout();
    
    ///
    /// Starts a frame - which is an encompassing region designed to act as an
    /// Event subscriber for views, or as a container for other ui items.
    ///
    Layout& frame(int id, unsigned int eventFlags, FrameHandler* frameHandler,
                  RenderCallback renderCb=nullptr, void* cbContext=nullptr,
                  unsigned int layoutFlags=0);
    ///
    /// Starts a vertical group of ui widgets - used for positioning.
    ///
    Layout& column(unsigned int layoutFlags=UI_FILL);
    ///
    /// Sets the size of the current UI region.
    ///
    Layout& setSize(int w, int h);
    ///
    /// An individual button widget within the encompassing UI group.
    ///
    Layout& button(int id, ButtonHandler* btnHandler, int iconId, const char* label);
    ///
    /// A list box whose data is updated by events sent to the subscriber
    ///
    Layout& listbox(DataProvider* dataProvider, int providerId,
                    ListboxLayout layout,
                    int* selected);
    ///
    /// Completes a layout group (frame, column)
    ///
    Layout& end();
    
    /// Returns the current layout group item.  Note that this returns the item
    /// index for frame, column, row, and other groupings.  This method does not
    /// return items defined in a single line (line items like buttons, and
    /// other atomic widgets.)
    ///
    int currentGroupItem() const { return _topItem; }
    
private:
    static constexpr size_t kDataSize = sizeof(vector<int32_t>);
    static constexpr size_t kMaxItemsInData = kDataSize / sizeof(int32_t);
    
    uint8_t _data[kDataSize];
    uint32_t _size;             // high bit, then _data contains a vector
    int _topItem;
    
    enum
    {
        kFreeLayout,
        kColumnLayout
    }
    _mode;
    
    void insertItem(int32_t item, int32_t parent);
    void pushTop();
    int popItem();
};


} /* namespace uicore */ } /* namespace cinek */

#endif /* defined(UI_Builder) */
