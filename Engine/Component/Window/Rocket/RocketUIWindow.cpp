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

#include "./RocketUIWindow.hpp"
#include "./RocketSDLInputMap.hpp"

#include "Rocket/Core/Input.h"

namespace cinekine {
    namespace ovengine {

    RocketUIWindow::RocketUIWindow(Rocket::Core::ElementDocument* document,
                                   const RocketSDLInputMap& inputMapper) :
        _document(document),
        _eventListener(nullptr),
        _inputMap(inputMapper)
    {
        _document->AddEventListener("keydown", this);
        _document->AddEventListener("keyup", this);
    }
    
    RocketUIWindow::~RocketUIWindow()
    {
        _document->Close();
        _document->RemoveReference();
    }

    void RocketUIWindow::show()
    {
         _document->Show();
    }

    void RocketUIWindow::ProcessEvent(Rocket::Core::Event& event)
    {
        if (!_eventListener)
            return;

        if (event == "keydown" || event == "keyup")
        {

            Rocket::Core::Input::KeyIdentifier key =
                (Rocket::Core::Input::KeyIdentifier)event.GetParameter<int>(
                                                        "key_identifier", 
                                                        Rocket::Core::Input::KI_UNKNOWN);
            bool ctrlKey = event.GetParameter("ctrl_key", false);
            bool shiftKey = event.GetParameter("shift_key", false);
            bool altKey = event.GetParameter("alt_key", false);

            uint16_t mod = 0;
            if (ctrlKey)
                mod |= Rocket::Core::Input::KM_CTRL;
            else if (shiftKey)
                mod |= Rocket::Core::Input::KM_SHIFT;
            else if (altKey)
                mod |= Rocket::Core::Input::KM_ALT;

            RocketSDLInputMap::SDLKey sdlKey = _inputMap.translateRocketKey(key, mod);

            if (event == "keydown")
            {
                _eventListener->onKeyDown(sdlKey.code, sdlKey.mod);
            }
            else
            {
                _eventListener->onKeyUp(sdlKey.code, sdlKey.mod);
            }
        }
    }

    }   // namespace ovengine
}   // namespace cinekine
