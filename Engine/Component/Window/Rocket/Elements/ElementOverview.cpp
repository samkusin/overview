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

#include "ElementOverview.hpp"
#include "../RocketServer.hpp"

namespace cinekine {
    namespace ovengine {

    RocketElementOverview::RocketElementOverview(const Rocket::Core::String& tag,
                                                 const RocketSDLInputMap& inputMap,
                                                 std::shared_ptr<View> view) :
        Rocket::Core::Element(tag),
        _inputMap(inputMap),
        _view(view)
    {
     //   AddEventListener("mousedown", this, true);
    }
    
    RocketElementOverview::~RocketElementOverview()
    {
    }

    void RocketElementOverview::ProcessEvent(Rocket::Core::Event& event)
    {
        Rocket::Core::Element::ProcessEvent(event);

        if (!_view)
            return;

        if (event == "mousemove" || event == "mouseover" || event == "mouseout")
        {
            int32_t mx = event.GetParameter("mouse_x", 0);
            int32_t my = event.GetParameter("mouse_y", 0);
            View::MouseRegion region = View::kMouseRegionOver;
            if (event == "mouseover")
                region = View::kMouseRegionEnter;
            else if (event == "mouseout")
                region = View::kMouseRegionExit;
   
            _view->onMouseMove(region, mx, my);
        }
        else if (event == "mousedown" || event == "mouseup")
        {
            int32_t mx = event.GetParameter("mouse_x", 0);
            int32_t my = event.GetParameter("mouse_y", 0);
            int32_t mb = event.GetParameter("button", -1);
            if (mb >= 0)
            {
                View::MouseButton button = View::kMouseButtonLeft;
                if (mb == 1)
                {
                    button = View::kMouseButtonRight;
                }
                else if (mb == 2)
                {
                    button = View::kMouseButtonCenter;
                }
                else if (mb > 2)
                {
                    //  unhandled
                    return;
                }
                if (event == "mousedown")
                {
                    _view->onMouseButtonDown(button, mx, my);
                }
                else
                {
                    _view->onMouseButtonUp(button, mx, my);
                }
            }   
        }
        else if (event == "keydown" || event == "keyup")
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
                _view->onKeyDown(sdlKey.code, sdlKey.mod);
            }
            else
            {
                _view->onKeyUp(sdlKey.code, sdlKey.mod);
            }
        }

    }

    void RocketElementOverview::OnRender()
    {
        if (_view)
        {
            _view->render();
        }
    }


    }   // namespace ovengine
}   // namespace cinekine
