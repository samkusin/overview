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

#include "./RocketServer.hpp"
#include "./RocketUIWindow.hpp"
#include "./RocketElementInstancers.hpp"

#include "Rocket/Core.h"
#include "Rocket/Core/Input.h"

namespace cinek {
    namespace overview {

    RocketServer::RocketServer(glx::RendererCLI& renderer, const Allocator& allocator) :
        _allocator(allocator),
        _rocketSystem(),
        _rocketFile(),
        _rocketRenderer(renderer, _allocator),
        _coreInitialized(false),
        _sdlInput(),
        _context(nullptr),
        _overviewElementInstancer(nullptr)
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

        /**
         * @todo use allocator instead of new/delete - this may require a refactor of how libRocket
         * allocates memory
         */
        _overviewElementInstancer = _allocator.newItem<RocketOverviewElementInstancer, RocketServer&, const Allocator&>
                                        (
                                            *this,
                                            _allocator
                                        );
        Rocket::Core::Factory::RegisterElementInstancer("overview", _overviewElementInstancer);

        glx::Rect viewport = _rocketRenderer.getViewport();
        _context = Rocket::Core::CreateContext( "default",
                                                Rocket::Core::Vector2i(viewport.width(),
                                                                       viewport.height())
                                              );
    }

    RocketServer::~RocketServer()
    {
        if (_context)
        {
            _context->RemoveReference();
            _context = nullptr;
        }
        if (_overviewElementInstancer)
        {
            _overviewElementInstancer->RemoveReference();
            _overviewElementInstancer = nullptr;
        }
        if (_coreInitialized)
        {
            Rocket::Core::Shutdown();
        }
    }

    RocketServer::operator bool() const
    {
        return _coreInitialized && _context;
    }

    void RocketServer::update(uint32_t currentTime)
    {
        _rocketSystem.setCurrentTime(currentTime/1000.0f);
        if (_context)
        {
            _context->Update();
        }
    }

    void RocketServer::render()
    {
        if (_context)
        {
            _context->Render();
        }
    }


    void RocketServer::handleInput(const SDL_Event& event)
    {
        _sdlInput.dispatchSDLEvent(event, _context);
    }

    WindowPtr RocketServer::createWindow(const char* name,
                                         ViewCreateFn createDelegate)
    {
        /**
         * @todo Move createWindow to a factory for UIWindow objects
         */
        WindowPtr ptr;
        if (_context)
        {
            _overviewElementInstancer->setViewRequestDelegate(createDelegate);
            Rocket::Core::ElementDocument *uiDocument = _context->LoadDocument(name);
            if (uiDocument)
            {
                //  create our window and views
                ptr = std::allocate_shared<RocketUIWindow, std_allocator<RocketUIWindow>,
                                            Rocket::Core::ElementDocument*>
                                    (
                                        std_allocator<RocketUIWindow>(_allocator),
                                        std::move(uiDocument)
                                    );
            }
            uiDocument->RemoveReference();
            _overviewElementInstancer->clearViewRequestDelegate();
        }
        return ptr;
    }


    }   // namespace overview
}   // namespace cinek
