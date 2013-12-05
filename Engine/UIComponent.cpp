/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 */

#include "Component/UIComponentImpl.hpp"
#include "Component/Rocket/RocketUI.hpp"

namespace cinekine {
    namespace ovengine {
   
    UIComponent::UIComponent(glx::Renderer& renderer, Allocator& allocator) :
        _impl(allocator.newItem<RocketUI>(renderer, allocator))
    {
    }

    UIComponent::~UIComponent()
    {
    }

    UIComponent::operator bool() const
    {
        return *(_impl.get());
    }

    void UIComponent::update(cinek_time currentTime)
    {
        _impl->update(currentTime);
    }

    void UIComponent::render()
    {
        _impl->render();
    }
    
    void UIComponent::handleInput(const SDL_Event& event)
    {
        return _impl->handleInput(event);
    }

    unique_ptr<UIWindow> UIComponent::createWindow(const char* name)
    {
        return std::move(unique_ptr<UIWindow>(_impl->createWindow(name)));
    }

    }   // namespace ovengine
}   // namespace cinekine
