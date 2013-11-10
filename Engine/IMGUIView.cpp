//
//  IMGUIView.cpp
//  Overview
//
//  Created by Samir Sinha on 9/25/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#include "IMGUIView.hpp"
#include "Render/Renderer.hpp"

namespace cinekine {
    namespace ovengine {

    IMGUIView::IMGUIView(glx::Renderer& renderer) :
        _renderer(renderer)
    {

    }

    IMGUIView::~IMGUIView()
    {

    }

    void IMGUIView::render(const imgui::DrawCommands& drawCommands)
    {

    }

    }   // namespace ovengine
}   // namespace cinekine

