//
//  IMGUIView.hpp
//  Overview
//
//  Created by Samir Sinha on 9/25/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_IMGUIView_hpp
#define Overview_IMGUIView_hpp

#include "UI/IMGUITypes.hpp"

namespace cinekine {
    namespace glx {
        class Renderer;
    }
    namespace ovengine {

    //  The IMGUIView is responsible for translating imgui DrawCommands
    //  to Renderer calls.  It belongs to the ovengine namespace since
    //  per-application visual customization should be done via style
    //  sheets or some other markup.
    //  
    class IMGUIView
    {
    public:
        IMGUIView(glx::Renderer& renderer);
        ~IMGUIView();

        void render(const imgui::DrawCommands& drawCommands);

    private:
        glx::Renderer& _renderer;
    };

    }   // namespace ovengine
}   // namespace cinekine

#endif
