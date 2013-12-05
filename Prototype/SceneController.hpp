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

#ifndef Overview_SceneController_hpp
#define Overview_SceneController_hpp

#include "Scene.hpp"

#include "cinek/cpp/ckalloc.hpp"
#include "cinek/cpp/ckstring.hpp"
#include "cinek/cpp/ckmap.hpp"

#include <functional> 

namespace cinekine {
    namespace prototype {

    class SceneController
    {
    public:
        SceneController(const Allocator& allocator);
        ~SceneController();

        typedef std::function<std::shared_ptr<Scene>(void)> SceneCreateFn;

        //  add a scene provider - the provider will supply a Scene object that will
        //  be managed by the Scenes collection during state changes
        void add(const char* name, SceneCreateFn createFn);
        //  advances to the next Scene
        void next(const char* name);
        //  updates the current Scene
        void update();

    private:
        unordered_map<string, SceneCreateFn> _sceneMap;
        std::shared_ptr<Scene> _currentScene;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif
