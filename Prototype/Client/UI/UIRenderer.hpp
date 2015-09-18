//
//  UIRenderer.hpp
//  Overview
//
//  Created by Samir Sinha on 7/20/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_UI_Renderer_hpp
#define Overview_UI_Renderer_hpp

#include "UITypes.hpp"

#include <ckentity/entity.h>

namespace cinek {

namespace ovproto {

class Diagnostics;

NVGcontext* createUIRenderingContext(int viewId);

void destroyUIRenderingContext(NVGcontext* nvg);

void renderUI(NVGcontext* context, const gfx::Rect& viewRect);

void renderDiagnostics(Diagnostics& diagnostics,
                       EntityDiagnostics& entityDiagnostics,
                       NVGcontext* context, const gfx::Rect& viewRect);

} /* namespace ovproto */ } /* namespace cinek */

#endif /* defined(UIRenderer) */
