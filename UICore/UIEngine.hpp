//
//  UIEngine.hpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

//
// ImGui Framework
// https://github.com/ocornut/imgui
//
// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Omar Cornut and ImGui contributors
//
// License: https://github.com/ocornut/imgui/blob/master/LICENSE
//
// Changes: Modified ImGui Sample to support BGFX
//

#ifndef Overview_UI_Engine_hpp
#define Overview_UI_Engine_hpp

struct SDL_Window;
typedef union SDL_Event SDL_Event;

bool imGuiInit(SDL_Window *window, int viewId);
void imGuiShutdown();
void imGuiNewFrame();
bool imGuiProcessEvent(SDL_Event* event);
void imGuiRender();

// Use if you want to reset your rendering device without losing ImGui state.
void imGuiInvalidateDeviceObjects();
bool imGuiCreateDeviceObjects();


#endif /* defined(Overview_UI_Engine_hpp) */
