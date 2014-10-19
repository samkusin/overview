//
//  Overview.cpp
//  Overview
//
//  Created by Samir Sinha on 8/22/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "Overview.hpp"
#include "ApplicationController.hpp"
#include "View/GameView.hpp"

#include "cinek/allocator.hpp"

namespace cinek {
    namespace overview {

    overview::Director* CreateDirector(WindowComponentCLI& windowCLI,
                                       glx::RendererCLI& rendererCLI)
    {
        Allocator allocator;
        return allocator.newItem<overview::Overview>(windowCLI, rendererCLI);
    }

    void DestroyDirector(overview::Director* director)
    {
        Allocator allocator;
        allocator.deleteItem(director);
    }

    }   // namespace overview
}

namespace cinek {
    namespace overview {

    Overview::Overview(overview::WindowComponentCLI& uiCLI,
                       glx::RendererCLI& rendererCLI) :
        _allocator(),
        _controller(uiCLI, rendererCLI, _allocator)
    {
        _controller.add( "game",
            [this](ApplicationController& app, const Allocator& allocator) -> std::shared_ptr<overview::View>
            {
                return std::allocate_shared<GameView,
                                            std_allocator<GameView>,
                                            ApplicationController&,
                                            const Allocator&>
                                        (
                                            allocator,
                                            app,
                                            allocator
                                        );
                });

        _controller.next("game");
    }

    Overview::~Overview()
    {
    }

    void Overview::update(uint32_t ticks)
    {
        _controller.update(ticks);
    }


    }   //  namespace overview
}   // namespace cinek
