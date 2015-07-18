//
//  ViewController.hpp
//  Overview
//
//  Created by Samir Sinha on 7/17/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_View_Controller_hpp
#define Overview_View_Controller_hpp

#include <cinek/vector.hpp>
#include <cinek/allocator.hpp>
#include <functional>

namespace cinek { namespace overview {

class ViewController
{
public:
    virtual ~ViewController() {}
    
    //  Executed when a view controller is brought into active management
    virtual void onViewLoad() = 0;
    //  Executed when a view controller is removed from active management
    virtual void onViewUnload() = 0;
    //  Executed when a view controller is brought onto the stack
    virtual void onViewAdded() = 0;
    //  Executed when a view controller is taken off the stack
    virtual void onViewRemoved() = 0;
    //  Executed when a view is brought into the foreground, called when a view
    //  higher up on the stack is removed, bringing this view back into the
    //  foreground
    virtual void onViewForeground() = 0;
    //  Executed when a view is put into the background, usually called when a
    //  view is added to the stack, over the current view
    virtual void onViewBackground() = 0;
    //  Executed when a view requires layout (changes in dimension, or upon user
    //  input.  Typically this method is used to layout UI
    virtual void layoutView() = 0;
    //  Updates execution of the current view controller.  Execution occurs
    //  within an interval, but not necessary every frame.  For example, don't
    //  use update to trigger time sensitive operations (if that's needed, use
    //  timers and tasks.)
    virtual void updateView() = 0;
    //  Renders the view and is executed every frame.
    virtual void renderView() = 0;
    
    //  the name of the view
    virtual int viewId() const = 0;
};

} /* namespace overview */ } /* namespace cinek */


#endif /* defined(Overview_View_Controller_hpp) */