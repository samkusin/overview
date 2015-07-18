//
//  ViewStack.cpp
//  Overview
//
//  Created by Samir Sinha on 7/17/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "ViewStack.hpp"
#include "ViewController.hpp"
#include "Debug.hpp"

namespace cinek { namespace overview {

ViewStack::ViewStack(const Allocator& allocator) :
    _factoryCb(),
    _views(allocator),
    _stack(allocator),
    _commands(allocator)
{
    _views.reserve(32);
    _stack.reserve(8);
    _commands.reserve(64);
}

ViewStack::~ViewStack() = default;

void ViewStack::setFactory(const FactoryCallback& callback)
{
    _factoryCb = callback;
}
    
void ViewStack::layout()
{
    for (auto& vc : _stack)
    {
        vc->layoutView();
    }
}

void ViewStack::process()
{
    //  process commands on stack
    for (auto& cmd : _commands)
    {
        switch (cmd.type)
        {
        case Command::kLoad:
            {
                cmdLoad(cmd.viewId);
            }
            break;
        case Command::kPush:
            {
                if (viewIndex(cmd.viewId) < 0)
                {
                    auto view = cmdLoad(cmd.viewId);
                    if (view)
                    {
                        cmdPush(view, true);
                    }
                }
            }
            break;
        case Command::kPop:
            {
                cmdPop(true);
            }
            break;
        case Command::kPresent:
            {
                //  if on the view stack, pop everything above the view
                //  we want to re-present
                //
                int idx = viewIndex(cmd.viewId);
                if (idx >= 0)
                {
                    //  do nothing if view is already at the top, of course.
                    if (idx < _stack.size()-1)
                    {
                        //  pop without sending top views to foreground....
                        //  except for the last view to pop, which should
                        //  result in foregrounding the view to present.
                        while (_stack.size()-2 > idx)
                        {
                            cmdPop(false);
                        }
                        cmdPop(true);
                    }
                }
                else
                {
                    //  remove our topmost view, and add our new view
                    //  this should occur without foregrounding the view
                    //  below the one we're going to present.
                    cmdPop(false);
                    auto view = cmdLoad(cmd.viewId);
                    if (view)
                    {
                        cmdPush(view, false);
                    }
                }
            }
            break;
        case Command::kUnload:
            {
                //  if on the view stack, pop everything up to and including
                //  the view to unload.  We want to treat the last pop as
                //  foregrounding the view below it.
                int idx = viewIndex(cmd.viewId);
                if (idx >= 0)
                {
                    while (_stack.size() > (idx+1))
                    {
                        cmdPop(false);
                    }
                    
                    cmdPop(true);
                }
                else
                {
                    cmdUnload(cmd.viewId);
                }
            }
            break;
        }
    }
    
    _commands.clear();
    
    
    for (auto& vc : _stack)
    {
        vc->updateView();
    }
}

int ViewStack::viewIndex(int id) const
{
    for (auto it = _stack.begin(); it != _stack.end(); ++it)
    {
        if ((*it)->viewId() == id)
            return (int)(it - _stack.begin());
    }
    
    return -1;
}

ViewController* ViewStack::cmdLoad(int id)
{
    auto it = std::lower_bound(_views.begin(), _views.end(), id,
        [](const decltype(_views)::value_type& v, int vid) -> bool {
            return v.first->viewId() < vid;
        });
    if (it == _views.end() || it->first->viewId() != id)
    {
        auto view = _factoryCb(id);
        if (view)
        {
            it = _views.emplace(it, std::move(view), 0);
            it->first->onViewLoad();
        }
    }
    ++it->second;
    return it->first.get();
}

void ViewStack::cmdUnload(int id)
{
    auto it = std::lower_bound(_views.begin(), _views.end(), id,
        [](const decltype(_views)::value_type& v, int vid) -> bool {
            return v.first->viewId() < vid;
        });
    if (it == _views.end() || it->first->viewId() != id)
        return;
    
    --it->second;
    if (!it->second)
    {
        it->first->onViewUnload();
        _views.erase(it);
    }
}

void ViewStack::cmdPop(bool foregroundTop)
{
    if (_stack.empty())
        return;
    
    auto vc = _stack.back();
    vc->onViewRemoved();
    _stack.pop_back();

    cmdUnload(vc->viewId());
    
    if (!_stack.empty() && foregroundTop)
    {
        _stack.back()->onViewForeground();
    }
}

void ViewStack::cmdPush(ViewController* vc, bool backgroundTop)
{
    if (!_stack.empty() && backgroundTop)
    {
        _stack.back()->onViewBackground();
    }
    _stack.push_back(vc);
    vc->onViewAdded();
}

void ViewStack::render()
{
    // render everything
    for (auto& vc : _stack)
    {
        vc->renderView();
    }
}

void ViewStack::load(int id)
{
    _commands.push_back({Command::kLoad, id});
}

void ViewStack::unload(int id)
{
    _commands.push_back({Command::kUnload, id});
}

void ViewStack::present(int id)
{
    _commands.push_back({Command::kPresent, id});
}

void ViewStack::push(int id)
{
    _commands.push_back({Command::kPush, id});
}

void ViewStack::pop()
{
    _commands.push_back({Command::kPop,});
}

} /* namespace overview */ } /* namespace cinek */
