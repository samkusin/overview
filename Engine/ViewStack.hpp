//
//  ViewStack.hpp
//  Overview
//
//  Created by Samir Sinha on 7/17/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_View_Manager_hpp
#define Overview_View_Manager_hpp

#include <cinek/allocator.hpp>
#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <mutex>

namespace cinek { namespace ove {

class ViewController;

class ViewStack
{
public:
    using FactoryCallback = std::function<unique_ptr<ViewController>(const std::string&)>;
    
    ViewStack();
    ~ViewStack();
    
    void setFactory(FactoryCallback callback);
    
    //  processes view state changes (load, unload, etc.)
    void process();
    
    //  executes layout of view controllers
    void layout();
    
    //  updates the view controllers on the stack
    void simulate(double time, double dt);
    
    //  render the stack
    void frameUpdate(double dt);
    
    //  loads a view (or ups the reference count if already loaded)
    void load(const std::string& id);
    //  unloads a view (or decrements the ref count.)  if this is the last
    //  reference, and the view is on the stack, the view and its successors on
    //  the stack are also unloaded
    void unload(const std::string&  id);
    //  presents a view (executes load and open.), removing the topmost view
    //  and presenting it in place.  if this view is already on the stack,
    //  all views are popped above the presented view.
    void present(const std::string& id);
    //  pushes a view onto the stack.  the current view is backgrounded.  if the
    //  view is already on the stack, does nothing.
    void push(const std::string& id);
    //  pops the current view from the stack and unloads it (decrements ref
    //  count).
    void pop();
    //  resets the view stack, and unloads views
    //  views are removed (onViewRemoved) from back to front
    //  views are unloaded (onViewUnload) in no particular order
    void reset();
    
    //  Returns a read-only interface to the current ViewController.  This is
    //  meant to be called from a ViewController implementation and from the
    //  context of one of the 'action' methods of ViewStack (process, layout,
    //  simulate, frameUpdate.)  If called outside of these contexts, then the
    //  current controller will be null.
    const ViewController* currentController() const;
    
private:
    FactoryCallback _factoryCb;
    std::mutex _runMutex;
    std::vector<std::pair<unique_ptr<ViewController>, int>> _views;
    std::vector<ViewController*> _stack;
    
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
        std::string viewId;
    };
    
    std::vector<Command> _commands;
    
    int viewIndex(const std::string& id) const;
    ViewController* cmdLoad(const std::string& id);
    void cmdPop(bool foregroundTop);
    void cmdPush(ViewController* vc, bool backgroundTop);
    void cmdUnload(const std::string& id);
    
    std::thread::id _activeThread;
    ViewController* _activeController;
};

} /* namespace ove */ } /* namespace cinek */


#endif /* defined(Overview_View_Manager_hpp) */
