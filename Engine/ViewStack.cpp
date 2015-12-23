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

void ViewStack::startFrame()
{
    if (_activeThread == std::thread::id())
    {
        std::lock_guard<std::mutex> lock(_runMutex);
        _activeThread = std::this_thread::get_id();

        // render everything
        for (auto& vc : _stack)
        {
            _activeController = vc;
            _activeController->onViewStartFrame(*this);
        }
        _activeController = nullptr;
        _activeThread = std::thread::id();
    }
}
    
void ViewStack::endFrame()
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
            }
        }
        
        _commands.clear();
        
        // render everything
        for (auto& vc : _stack)
        {
            _activeController = vc;
            _activeController->onViewEndFrame(*this);
        }
        _activeController = nullptr;
        
        _activeThread = std::thread::id();
    }
}

void ViewStack::simulate(double dt)
{
    if (_activeThread == std::thread::id())
    {
        std::lock_guard<std::mutex> lock(_runMutex);
        _activeThread = std::this_thread::get_id();

        for (auto& vc : _stack)
        {
            _activeController = vc;
            _activeController->simulateView(*this, dt);
        }
        _activeController = nullptr;
        _activeThread = std::thread::id();
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
            _activeController->frameUpdateView(*this, dt);
        }
        _activeController = nullptr;
        _activeThread = std::thread::id();
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
        ViewController* parentController = nullptr;
        if (!_stack.empty()) {
            parentController = _stack.back();
        }
        auto view = _factoryCb(id, parentController);
        if (view)
        {
            it = _views.emplace(it, std::move(view), 0);
        }
        else
        {
            return nullptr;
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
        _views.erase(it);
    }
}

void ViewStack::cmdPop(bool foregroundTop)
{
    if (_stack.empty())
        return;
    
    auto vc = _stack.back();
    _activeController = vc;
    _activeController->onViewRemoved(*this);
    _activeController = nullptr;
    
    _stack.pop_back();

    cmdUnload(vc->viewId());
    
    if (!_stack.empty() && foregroundTop)
    {
        _activeController = _stack.back();
        _activeController->onViewForeground(*this);
        _activeController = nullptr;
    }
}

void ViewStack::cmdPush(ViewController* vc, bool backgroundTop)
{
    if (!_stack.empty() && backgroundTop)
    {
        _activeController = _stack.back();
        _activeController->onViewBackground(*this);
        _activeController = nullptr;
    }
    _stack.push_back(vc);

    _activeController = vc;
    _activeController->onViewAdded(*this);
    _activeController = nullptr;
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
