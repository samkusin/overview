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

#include "SceneController.hpp"
#include "Engine/Debug.hpp"

namespace cinekine {
    namespace prototype {

    SceneController::SceneController(ovengine::WindowComponentCLI& ui,
                                     glx::RendererCLI& renderer,
                                     const Allocator& allocator) :
        _ui(ui),
        _renderer(renderer),
        _sceneMap(std_allocator<std::pair<string, SceneCreateFn>>(allocator)),
        _currentScene()
    {

    }

    SceneController::~SceneController()
    {
    }

    void SceneController::add(const char* name, SceneCreateFn createFn)
    {
        _sceneMap[string(name)] = createFn;
    }

    void SceneController::next(const char* name)
    {
        auto sceneIt = _sceneMap.find(string(name));
        if (sceneIt == _sceneMap.end())
        {
            OVENGINE_LOG_ERROR("SceneController.next - scene '%s' not found", name);
            return;
        }
        std::shared_ptr<Scene> nextScene = (*sceneIt).second();
        if (!nextScene)
        {
            OVENGINE_LOG_ERROR("SceneController.next - scene '%s' creation failed", name);
            return;
        }
        _currentScene = nextScene;
    }

    void SceneController::update()
    {
        if (_currentScene)
        {
            _currentScene->update();
        }
    }

    }   // namespace ovengine
}   // namespace cinekine

