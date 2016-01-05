//
//  ViewController.hpp
//  Overview
//
//  Created by Samir Sinha on 7/17/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_View_Controller_hpp
#define Overview_View_Controller_hpp

#include "EngineTypes.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek { namespace ove {

class ViewController
{
public:
    virtual ~ViewController() {}
    
    //  Executed when a view controller is brought onto the stack
    virtual void onViewAdded(ViewStack& stateController) = 0;
    //  Executed when a view controller is taken off the stack
    virtual void onViewRemoved(ViewStack& stateController) = 0;
    //  Executed when a view is brought into the foreground, called when a view
    //  higher up on the stack is removed, bringing this view back into the
    //  foreground
    virtual void onViewForeground(ViewStack& stateController) = 0;
    //  Executed when a view is put into the background, usually called when a
    //  view is added to the stack, over the current view
    virtual void onViewBackground(ViewStack& stateController) = 0;
    //  Executed upon the the start of a frame
    virtual void onViewStartFrame(ViewStack& stateController) = 0;
    //  Updates simulation of the current view controller.  Execution occurs
    //  within an interval, but not necessary every frame (or more than once a
    //  frame depending on the owning loop.)
    virtual void simulateView(ViewStack& stateController, double dt) = 0;
    //  Executed upon the the end of a frame
    virtual void onViewEndFrame(ViewStack& stateController) = 0;
    //  the name of the view
    virtual const char* viewId() const = 0;
};

} /* namespace ove */ } /* namespace cinek */


#endif /* defined(Overview_View_Controller_hpp) */
