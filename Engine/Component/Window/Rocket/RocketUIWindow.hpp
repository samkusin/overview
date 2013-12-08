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

#ifndef Overview_Components_Rocket_UIWindow_hpp
#define Overview_Components_Rocket_UIWindow_hpp

#include "Engine/Component/Window/Window.hpp"

#include "Rocket/Core/ElementDocument.h"
#include "Rocket/Core/EventListener.h"

namespace cinekine {
    namespace ovengine {

    class RocketSDLInputMap;

    class RocketUIWindow :
        public Window,
        public Rocket::Core::EventListener
    {
    public:
        RocketUIWindow(Rocket::Core::ElementDocument* document,
                       const RocketSDLInputMap& inputMapper);
        virtual ~RocketUIWindow();

        virtual void show();
        virtual void setEventListener(WindowEventListener* listener) {
            _eventListener = listener;
        }

        virtual void ProcessEvent(Rocket::Core::Event& event);

    private:
        Rocket::Core::ElementDocument* _document;
        WindowEventListener* _eventListener;
        const RocketSDLInputMap& _inputMap;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif