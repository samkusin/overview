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

#include "RocketUI.hpp"
#include "RocketUIWindow.hpp"
#include "RocketSDLInput.hpp"
#include "RocketFileInterface.hpp"
#include "RocketSystemInterface.hpp"
#include "RocketRenderInterface.hpp"

#include "Rocket/Core.h"
#include "Rocket/Core/Input.h"

namespace cinekine {
    namespace ovengine {

    class RocketUI::Impl
    {
    public:
        Impl(glx::Renderer& renderer, Allocator& allocator) :
            _allocator(allocator),
            _rocketSystem(),
            _rocketFile(),
            _rocketRenderer(renderer),
            _coreInitialized(false),
            _sdlInput(),
            _context(nullptr)
        {
            Rocket::Core::SetFileInterface(&_rocketFile);
            Rocket::Core::SetRenderInterface(&_rocketRenderer);
            Rocket::Core::SetSystemInterface(&_rocketSystem);

            _coreInitialized = Rocket::Core::Initialise();
            if (!_coreInitialized)
                return;
    
            Rocket::Core::FontDatabase::LoadFontFace("static/fonts/Delicious-Bold.otf");
            Rocket::Core::FontDatabase::LoadFontFace("static/fonts/Delicious-BoldItalic.otf");
            Rocket::Core::FontDatabase::LoadFontFace("static/fonts/Delicious-Italic.otf");
            Rocket::Core::FontDatabase::LoadFontFace("static/fonts/Delicious-Roman.otf");

            glx::Rect viewport = _rocketRenderer.getViewport();

            _context = Rocket::Core::CreateContext( "default",
                                                    Rocket::Core::Vector2i(viewport.width(), 
                                                                           viewport.height())
                                                  );
        }

        ~Impl()
        {
            if (_context)
            {
                _context->RemoveReference();
                _context = nullptr;
            }
            if (_coreInitialized)
            {
                Rocket::Core::Shutdown();
            }
        }

        operator bool() const
        {
            return _coreInitialized && _context;
        }

        void update(cinek_time currentTime)
        {
            _rocketSystem.setCurrentTime(currentTime/1000.0f);
            if (_context)
            {
                _context->Update();
            }
        }

        void render()
        {
            if (_context)
            {
                _context->Render();
            }
        }


        void handleInput(const SDL_Event& event)
        {
            _sdlInput.dispatchSDLEvent(event, _context);
        }

        UIWindow* createWindow(const char* name)
        {
            /**
             * @todo Move createWindow to a factory for UIWindow objects
             */
            if (_context)
            {
                Rocket::Core::ElementDocument *uiDocument = _context->LoadDocument(name);
                if (uiDocument)
                {
                    RocketUIWindow* windowImpl =
                        _allocator.newItem<RocketUIWindow, Rocket::Core::ElementDocument*>(
                            std::move(uiDocument)
                        );
                    if (windowImpl)
                    {
                        UIWindow* window = _allocator.newItem<UIWindow, const Allocator&, UIWindow::Impl*>(
                                _allocator, windowImpl
                            );
                        return window;
                    }
                }
            }
            return nullptr;
        } 

    private:
        Allocator _allocator;
        RocketSystemInterface _rocketSystem;
        RocketFileInterface _rocketFile;
        RocketRenderInterface _rocketRenderer;
        bool _coreInitialized;

        RocketSDLInput _sdlInput;
        Rocket::Core::Context *_context;
    };


    RocketUI::RocketUI(glx::Renderer& renderer, Allocator& allocator) :
        _impl(allocator.newItem<Impl>(renderer, allocator))
    {
    }

    RocketUI::~RocketUI()
    {
    }

    RocketUI::operator bool() const
    {
        return *(_impl.get());
    }

    void RocketUI::update(cinek_time currentTime)
    {
        _impl->update(currentTime);
    }

    void RocketUI::render()
    {
        _impl->render();
    }
    
    void RocketUI::handleInput(const SDL_Event& event)
    {
        return _impl->handleInput(event);
    }

    unique_ptr<UIWindow> RocketUI::createWindow(const char* name)
    {
        return std::move(unique_ptr<UIWindow>(_impl->createWindow(name)));
    }

    }   // namespace ovengine
}   // namespace cinekine
