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

namespace cinek {

    namespace uicore {

NVGcontext* createRenderingContext(int viewId);

void destroyRenderingContext(NVGcontext* nvg);

void render(const Theme& theme, NVGcontext* nvg, const gfx::Rect& viewRect);

    } /* namespace uicore */

} /* namespace cinek */

#endif /* defined(UIRenderer) */
