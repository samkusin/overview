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
                                                 std::shared_ptr<View> view) :
        Rocket::Core::Element(tag),
        _view(view)
    {

    }
    
    RocketElementOverview::~RocketElementOverview()
    {
    }

    void RocketElementOverview::ProcessEvent(Rocket::Core::Event& event)
    {
        Rocket::Core::Element::ProcessEvent(event);
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
