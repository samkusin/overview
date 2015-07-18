//
//  ViewStack.hpp
//  Overview
//
//  Created by Samir Sinha on 7/17/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_View_Manager_hpp
#define Overview_View_Manager_hpp

#include <cinek/vector.hpp>
#include <cinek/allocator.hpp>
#include <functional>

namespace cinek { namespace overview {

class ViewController;

class ViewStack
{
public:
    using FactoryCallback = std::function<unique_ptr<ViewController>(int)>;
    
    ViewStack(const Allocator& allocator=Allocator());
    ~ViewStack();
    
    void setFactory(const FactoryCallback& callback);
    
    //  a re-layout of all views on the stack is required
    void layout();
    
    //  process any pending updates
    void process();
    
    //  render the stack
    void render();
    
    //  loads a view (or ups the reference count if already loaded)
    void load(int id);
    //  unloads a view (or decrements the ref count.)  if this is the last
    //  reference, and the view is on the stack, the view and its successors on
    //  the stack are also unloaded
    void unload(int id);
    //  presents a view (executes load and open.), removing the topmost view
    //  and presenting it in place.  if this view is already on the stack,
    //  all views are popped above the presented view.
    void present(int id);
    //  pushes a view onto the stack.  the current view is backgrounded.  if the
    //  view is already on the stack, does nothing.
    void push(int id);
    //  pops the current view from the stack and unloads it (decrements ref
    //  count).
    void pop();
    
private:
    FactoryCallback _factoryCb;
    vector<std::pair<unique_ptr<ViewController>, int>> _views;
    vector<ViewController*> _stack;
    
    struct Command
    {
        enum
        {
            kLoad,
            kUnload,
            kPresent,
            kPush,
            kPop
        }
        type;
        int viewId;
    };
    
    vector<Command> _commands;
    
    int viewIndex(int id) const;
    ViewController* cmdLoad(int id);
    void cmdPop(bool foregroundTop);
    void cmdPush(ViewController* vc, bool backgroundTop);
    void cmdUnload(int id);
};

} /* namespace overview */ } /* namespace cinek */


#endif /* defined(Overview_View_Manager_hpp) */
