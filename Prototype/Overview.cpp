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

namespace cinekine {
    namespace ovengine {

    ovengine::Director* CreateDirector(WindowComponentCLI& windowCLI,
                                       glx::RendererCLI& rendererCLI)
    {
        Allocator allocator;
        return allocator.newItem<prototype::Overview>(windowCLI, rendererCLI);
    }

    void DestroyDirector(ovengine::Director* director)
    {
        Allocator allocator;
        allocator.deleteItem(director);
    }

    }   // namespace ovengine
}

namespace cinekine {
    namespace prototype {

    Overview::Overview(ovengine::WindowComponentCLI& uiCLI,
                       glx::RendererCLI& rendererCLI) :
        _allocator(),
        _controller(uiCLI, rendererCLI, _allocator)
    {
        _controller.add( "game",
            [this](ApplicationController& app, const Allocator& allocator) -> std::shared_ptr<ovengine::View>
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


    }   //  namespace prototype
}   // namespace cinekine
