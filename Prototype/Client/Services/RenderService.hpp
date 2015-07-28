//
//  RenderService.hpp
//  Overview
//
//  Created by Samir Sinha on 7/7/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Service_Render_hpp
#define Overview_App_Service_Render_hpp

#include "Client/AppContext.hpp"
#include "Client/Render/RenderDefines.hpp"

namespace cinek {
    namespace ovproto {

class RenderService
{
public:
    RenderService() = default;
    RenderService(AppContext& context);
    
    //  Texture API
    gfx::TextureHandle loadTexture(const char* pathname);
    void unloadTexture(gfx::TextureHandle handle);
    bgfx::TextureHandle bgfxTextureFromHandle(gfx::TextureHandle handle) const;

    //  obtain the global store of rendering resources
    overview::RenderResources& renderResources();
    //  returns the main viewport rectangle
    gfx::Rect viewRect() const;
    //  obtain an instance of a 2D renderer bound to a specific view
    NVGcontext* obtain2DAPI();

private:
    AppObjects* _context = nullptr;
};
    
    } /* namespace ovproto */
} /* namespace cinek */

#endif