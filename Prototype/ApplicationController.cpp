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

#include "ApplicationController.hpp"
#include "Engine/Debug.hpp"
#include "Engine/View.hpp"

namespace cinekine {
    namespace prototype {

    ApplicationController::ApplicationController(ovengine::WindowComponentCLI& ui,
                                                 glx::RendererCLI& renderer,
                                                 const Allocator& allocator) :
        _ui(ui),
        _renderer(renderer),
        _allocator(allocator),
        _viewCreateMap(allocator)
    {

    }

    ApplicationController::~ApplicationController()
    {
    }

    void ApplicationController::add(const char* name, ViewCreateFn createFn)
    {
        _viewCreateMap[string(name)] = createFn;
    }

    void ApplicationController::next(const char* name)
    {
        auto viewIt = _viewCreateMap.find(string(name));
        if (viewIt == _viewCreateMap.end())
        {
            OVENGINE_LOG_ERROR("ApplicationController.next - scene '%s' not found", name);
            return;
        }
        auto nextView = (*viewIt).second(*this, _allocator);
        string rmlPath = "static/ui/";
        rmlPath += name;
        rmlPath += ".rml";
        _window = _ui.createWindow(rmlPath.c_str(),
                                   [this, nextView](const char* className, const char* idName)
                                   {
                                       return nextView;
                                   });
        if (_window)
        {
            _window->show();
        }

        _currentView = nextView;
    }

    void ApplicationController::update(uint32_t ticks)
    {
        if (_currentView)
        {
            _currentView->update(ticks);
        }
    }

    }   // namespace ovengine
}   // namespace cinekine

