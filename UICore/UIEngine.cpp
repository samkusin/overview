//
//  UIEngine.cpp
//  Overview
//
//  Created by Samir Sinha on 7/21/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "UIEngine.hpp"

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

#include "External/imgui/imgui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

//  must include after SDL
#include <bx/fpumath.h>
#include <bgfx/bgfx.h>

#include "vs_imgui.bin.h"
#include "fs_imgui.bin.h"

// SDL Events Data
static SDL_Window*  g_Window = NULL;
static double       g_Time = 0.0f;
static bool         g_MousePressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;

// BGFX Data
union ImGuiBGFXTextureLink
{
    void* ptr;
    bgfx::TextureHandle handle;

    bool isValid() const {
        return handle.idx != bgfx::invalidHandle;
    }
};

static int g_ViewId = 0;
static bgfx::VertexDecl    g_decl;
static bgfx::TextureHandle g_FontTexture = BGFX_INVALID_HANDLE;
static bgfx::ProgramHandle g_Program = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_TextureSampler = BGFX_INVALID_HANDLE;

static const char* imGuiGetClipboardText()
{
    return SDL_GetClipboardText();
}

static void imGuiSetClipboardText(const char* text)
{
    SDL_SetClipboardText(text);
}

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void imGuiRenderDrawLists(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;

    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    float ortho[16];
    bx::mtxOrtho(ortho, 0.0f, fb_width, fb_height, 0.0f, -1.0f, 1.0f);

    bgfx::setViewTransform(g_ViewId, NULL, ortho);

    // Render command lists
    for (int32_t ii = 0; ii < draw_data->CmdListsCount; ++ii)
    {
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        const ImDrawList* cmd_list   = draw_data->CmdLists[ii];
        uint32_t vtx_size = (uint32_t)cmd_list->VtxBuffer.size();
        uint32_t idx_size = (uint32_t)cmd_list->IdxBuffer.size();

        if (!bgfx::checkAvailTransientVertexBuffer(vtx_size, g_decl) || !bgfx::checkAvailTransientIndexBuffer(idx_size) )
        {
            // not enough space in transient buffer just quit drawing the rest...
            break;
        }

        bgfx::allocTransientVertexBuffer(&tvb, vtx_size, g_decl);
        bgfx::allocTransientIndexBuffer(&tib, idx_size);

        ImDrawVert* verts = (ImDrawVert*)tvb.data;
        memcpy(verts, cmd_list->VtxBuffer.begin(), vtx_size * sizeof(ImDrawVert) );

        ImDrawIdx* indices = (ImDrawIdx*)tib.data;
        memcpy(indices, cmd_list->IdxBuffer.begin(), idx_size * sizeof(ImDrawIdx) );

        uint32_t elem_offset = 0;
        const ImDrawCmd* pcmd_begin = cmd_list->CmdBuffer.begin();
        const ImDrawCmd* pcmd_end   = cmd_list->CmdBuffer.end();
        for (const ImDrawCmd* pcmd = pcmd_begin; pcmd != pcmd_end; pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
                elem_offset += pcmd->ElemCount;
                continue;
            }
            if (0 == pcmd->ElemCount)
            {
                continue;
            }

            bgfx::setState(0
                | BGFX_STATE_RGB_WRITE
                | BGFX_STATE_ALPHA_WRITE
                | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                | BGFX_STATE_MSAA
                );
            bgfx::setScissor(uint16_t(bx::fmax(pcmd->ClipRect.x, 0.0f) )
                , uint16_t(bx::fmax(pcmd->ClipRect.y, 0.0f) )
                , uint16_t(bx::fmin(pcmd->ClipRect.z, 65535.0f)-bx::fmax(pcmd->ClipRect.x, 0.0f) )
                , uint16_t(bx::fmin(pcmd->ClipRect.w, 65535.0f)-bx::fmax(pcmd->ClipRect.y, 0.0f) )
                );
            ImGuiBGFXTextureLink texture = { pcmd->TextureId };

            bgfx::setTexture(0, g_TextureSampler, texture.handle);

            bgfx::setVertexBuffer(&tvb, 0, vtx_size);
            bgfx::setIndexBuffer(&tib, elem_offset, pcmd->ElemCount);
            bgfx::submit(g_ViewId, g_Program);

            elem_offset += pcmd->ElemCount;
        }
    }
}


bool imGuiInit(SDL_Window *window, int viewId)
{
    g_Window = window;
    g_ViewId = viewId;
    //bgfx::sdlSetWindow(window);

    ImGuiIO& io = ImGui::GetIO();
    
    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;
    
    io.IniFilename = NULL;

    io.RenderDrawListsFn = imGuiRenderDrawLists;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.SetClipboardTextFn = imGuiSetClipboardText;
    io.GetClipboardTextFn = imGuiGetClipboardText;

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#endif

    /*
    bgfx::init();
    
    int w, h;
    SDL_GetWindowSize(g_Window, &w, &h);
    ImGui_ImplSdlBGFX_Reset(w, h);
    */
    
    g_decl.begin()
        .add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
    .end();

    return true;
}

void imGuiShutdown()
{
    imGuiInvalidateDeviceObjects();
    ImGui::Shutdown();
}


void imGuiNewFrame()
{
    if (!bgfx::isValid(g_FontTexture)) {
        imGuiCreateDeviceObjects();
    }

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    SDL_GetWindowSize(g_Window, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // Setup time step
    Uint32  time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(g_Window) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    else
        io.MousePos = ImVec2(-1, -1);

    io.MouseDown[0] = g_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;      // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = g_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = g_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    g_MousePressed[0] = g_MousePressed[1] = g_MousePressed[2] = false;

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

    // Start the frame
    ImGui::NewFrame();
}

bool imGuiProcessEvent(SDL_Event* event)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type)
    {
    case SDL_MOUSEWHEEL:
    {
        if (event->wheel.y > 0)
            g_MouseWheel = 1;
        if (event->wheel.y < 0)
            g_MouseWheel = -1;
        return true;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
        if (event->button.button == SDL_BUTTON_LEFT) g_MousePressed[0] = true;
        if (event->button.button == SDL_BUTTON_RIGHT) g_MousePressed[1] = true;
        if (event->button.button == SDL_BUTTON_MIDDLE) g_MousePressed[2] = true;
        return true;
    }
    case SDL_TEXTINPUT:
    {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharactersUTF8(event->text.text);
        return true;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
        io.KeysDown[key] = (event->type == SDL_KEYDOWN);
        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        return true;
    }
    }
    return false;
}

void imGuiCreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bpp;

    // TODO - try an alpha 8 texture when prototype done
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bpp);

    // Upload texture to graphics system
    g_FontTexture = bgfx::createTexture2D(width, height, 1,
                            bgfx::TextureFormat::RGBA8,
                            0,
                            bgfx::copy(pixels, width*height*bpp)
                    );

    io.Fonts->SetTexID((void*)(intptr_t)g_FontTexture.idx);
}


bool imGuiCreateDeviceObjects()
{
    const bgfx::Memory* vsmem = bgfx::makeRef(vs_imgui_bin_h, sizeof(vs_imgui_bin_h));
    const bgfx::Memory* fsmem = bgfx::makeRef(fs_imgui_bin_h, sizeof(fs_imgui_bin_h));

    bgfx::ShaderHandle vsh = bgfx::createShader(vsmem);
    bgfx::ShaderHandle fsh = bgfx::createShader(fsmem);
    g_Program = bgfx::createProgram(vsh, fsh, true);
    g_TextureSampler = bgfx::createUniform("s_tex", bgfx::UniformType::Int1);

    //  load shaders - we rely on BGFX internals to provide the buffers we need
    //  on demand
    imGuiCreateFontsTexture();

    return true;
}


void imGuiInvalidateDeviceObjects()
{
    bgfx::destroyProgram(g_Program);
    g_Program.idx = bgfx::invalidHandle;

    if (bgfx::isValid(g_FontTexture)) {

        bgfx::destroyTexture(g_FontTexture);
        g_FontTexture.idx = bgfx::invalidHandle;
    }
}

void imGuiRender()
{
    //bgfx::setViewClear(g_ViewId, BGFX_CLEAR_COLOR, 0x000000ff, 1.0f, 0);
    bgfx::setViewRect(g_ViewId, 0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    ImGui::Render();
}






