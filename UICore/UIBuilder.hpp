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
    Layout(Context* context);
    ~Layout();
    
    struct Box
    {
        int l, t, r, b;
    };
    
    ///
    /// Starts a frame - which is an encompassing region designed to act as an
    /// Event subscriber for views, or as a container for other ui items.
    ///
    Layout& beginFrame(unsigned int eventFlags, FrameState* frameState,
                       RenderCallback renderCb=nullptr, void* cbContext=nullptr);
    ///
    /// Starts a horizontal group of ui widgets - used for grouping.
    ///
    Layout& beginRow();
    ///
    /// Starts a vertical group of ui widgets - used for grouping.
    ///
    Layout& beginColumn();
    ///
    /// Starts a vertical group of ui widgets - used for grouping.
    ///
    Layout& beginWindow(const char* title);
    ///
    /// Sets the size of the current UI region.
    ///
    Layout& setSize(int w, int h);
    ///
    /// Sets the margins of the current UI region.
    ///
    Layout& setMargins(int l, int t, int r, int b);
    ///
    /// Overrides the current item's layout
    ///
    Layout& setLayout(unsigned int layout);
    ///
    /// An individual button widget within the encompassing UI group.
    ///
    Layout& button(int id, ButtonHandler* btnHandler, int iconId, const char* label);
    ///
    /// A list box whose data is updated by events sent to the subscriber
    ///
    Layout& listbox(DataProvider* dataProvider, int providerId,
                    ListboxType lbtype,
                    ListboxState* state);
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
    Context* _context;
    struct State
    {
        int item;
        int innerItem;
        unsigned int layout;
    };

    static constexpr size_t kDataSize = sizeof(vector<State>) * 4;
    static constexpr size_t kMaxItemsInData = kDataSize / sizeof(State);
    
    uint8_t _data[kDataSize];
    uint32_t _size;             // high bit, then _data contains a vector
    
    int _topItem;
    int _innerItem;
    unsigned int _topLayout;
    
    void pushTop();
    void popItem();
};


} /* namespace uicore */ } /* namespace cinek */

#endif /* defined(UI_Builder) */
