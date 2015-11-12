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

namespace cinek { namespace ove {

ViewStack::ViewStack() :
    _factoryCb(),
    _activeThread(),
    _activeController(nullptr)
{
    _views.reserve(32);
    _stack.reserve(4);
    _commands.reserve(32);
}

ViewStack::~ViewStack()
{
    reset();
}

void ViewStack::setFactory(FactoryCallback callback)
{
    _factoryCb = std::move(callback);
}

void ViewStack::reset()
{
    std::lock_guard<std::mutex> lock(_runMutex);
    
    _activeThread = std::this_thread::get_id();
    
    while (!_stack.empty())
    {
        cmdPop(false);
    }
    
    while (!_views.empty())
    {
        cmdUnload(_views.back().first->viewId());
    }
    
    _activeThread = std::thread::id();
}
    
void ViewStack::process()
{
    if (_activeThread == std::thread::id())
    {
        std::lock_guard<std::mutex> lock(_runMutex);
        
        _activeThread = std::this_thread::get_id();
        
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
        
        _activeThread = std::thread::id();
    }
}

void ViewStack::layout()
{
    if (_activeThread == std::thread::id())
    {
        std::lock_guard<std::mutex> lock(_runMutex);

        _activeThread = std::this_thread::get_id();
        
        for (auto& vc : _stack)
        {
            _activeController = vc;
            _activeController->layoutView();
        }
        _activeController = nullptr;
        _activeThread = std::this_thread::get_id();
    }
}

void ViewStack::simulate(double time, double dt)
{
    if (_activeThread == std::thread::id())
    {
        std::lock_guard<std::mutex> lock(_runMutex);
        _activeThread = std::this_thread::get_id();

        for (auto& vc : _stack)
        {
            _activeController = vc;
            _activeController->simulateView(time, dt);
        }
        _activeController = nullptr;
        _activeThread = std::this_thread::get_id();
    }
}

void ViewStack::frameUpdate(double dt)
{
    if (_activeThread == std::thread::id())
    {
        std::lock_guard<std::mutex> lock(_runMutex);
        _activeThread = std::this_thread::get_id();

        // render everything
        for (auto& vc : _stack)
        {
            _activeController = vc;
            _activeController->frameUpdateView(dt);
        }
        _activeController = nullptr;
        _activeThread = std::this_thread::get_id();
    }
}

const ViewController* ViewStack::currentController() const
{
    if (_activeThread != std::thread::id() &&
        _activeThread == std::this_thread::get_id()) {
    
        return _activeController;
    }
    else {
        return nullptr;
    }
}

int ViewStack::viewIndex(const std::string& id) const
{
    for (auto it = _stack.begin(); it != _stack.end(); ++it)
    {
        if ((*it)->viewId() == id)
            return (int)(it - _stack.begin());
    }
    
    return -1;
}

ViewController* ViewStack::cmdLoad(const std::string& id)
{
    auto it = std::lower_bound(_views.begin(), _views.end(), id,
        [](const decltype(_views)::value_type& v, const std::string& vid) -> bool {
            return v.first->viewId() < vid;
        });
    if (it == _views.end() || it->first->viewId() != id)
    {
        auto view = _factoryCb(id);
        if (view)
        {
            it = _views.emplace(it, std::move(view), 0);
            _activeController = it->first.get();
            _activeController->onViewLoad();
            _activeController = nullptr;
        }
    }
    ++it->second;
    return it->first.get();
}

void ViewStack::cmdUnload(const std::string& id)
{
    auto it = std::lower_bound(_views.begin(), _views.end(), id,
        [](const decltype(_views)::value_type& v, const std::string& vid) -> bool {
            return v.first->viewId() < vid;
        });
    if (it == _views.end() || it->first->viewId() != id)
        return;
    
    --it->second;
    if (!it->second)
    {
        _activeController = it->first.get();
        _activeController->onViewUnload();
        _activeController = nullptr;
        _views.erase(it);
    }
}

void ViewStack::cmdPop(bool foregroundTop)
{
    if (_stack.empty())
        return;
    
    auto vc = _stack.back();
    _activeController = vc;
    _activeController->onViewRemoved();
    _activeController = nullptr;
    
    _stack.pop_back();

    cmdUnload(vc->viewId());
    
    if (!_stack.empty() && foregroundTop)
    {
        _activeController = _stack.back();
        _activeController->onViewForeground();
        _activeController = nullptr;
    }
}

void ViewStack::cmdPush(ViewController* vc, bool backgroundTop)
{
    if (!_stack.empty() && backgroundTop)
    {
        _activeController = _stack.back();
        _activeController->onViewBackground();
        _activeController = nullptr;
    }
    _stack.push_back(vc);

    _activeController = vc;
    _activeController->onViewAdded();
    _activeController = nullptr;
}

void ViewStack::load(const std::string& id)
{
    _commands.push_back({Command::kLoad, id});
}

void ViewStack::unload(const std::string&  id)
{
    _commands.push_back({Command::kUnload, id});
}

void ViewStack::present(const std::string&  id)
{
    _commands.push_back({Command::kPresent, id});
}

void ViewStack::push(const std::string&  id)
{
    _commands.push_back({Command::kPush, id});
}

void ViewStack::pop()
{
    _commands.push_back({Command::kPop,});
}


} /* namespace ove */ } /* namespace cinek */
