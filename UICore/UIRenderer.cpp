//
//  UIRenderer.cpp
//  Overview
//
//  Created by Samir Sinha on 7/20/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "UIRenderer.hpp"
#include "UIEngine.hpp"

#include "oui.h"

#include "CKGfx/External/nanovg/nanovg.h"

#define BLENDISH_IMPLEMENTATION
#include "blendish.h"

#include <bgfx/bgfx.h>

namespace cinek { namespace uicore {

NVGcontext* createRenderingContext(int viewId)
{
    NVGcontext* nvg = nvgCreate(1, viewId);
    bgfx::setViewSeq(viewId, true);
    
    int font = nvgCreateFont(nvg, "system", "ui/DejaVuSans.ttf");
    if (font >= 0)
    {
        bndSetFont(font);
    
        int icons = nvgCreateImage(nvg, "ui/blender_icons16.png", 0);
        if (icons > 0)
        {
            bndSetIconImage(icons);
            return nvg;
        }
    }
    
    nvgDelete(nvg);
    return nullptr;
}

void destroyRenderingContext(NVGcontext* nvg)
{
    nvgDelete(nvg);
}


static void renderItems(NVGcontext* context, int parent);
static void renderItemsVertical(NVGcontext* context, int parent);


static void renderListbox
(
    const OUIListBoxData* lbcontext,
    NVGcontext* nvg,
    const UIrect& rect,
    int item,
    int corners
)
{
    const float kCornerRadius = 3.0f;
 
    nvgSave(nvg);
    
    //  draw the box window
    nvgBeginPath(nvg);
    nvgRoundedRect(nvg, rect.x, rect.y, rect.w, rect.h, kCornerRadius);
    nvgFillColor(nvg, bnd_theme.backgroundColor);
    nvgFill(nvg);
    nvgRestore(nvg);
}


static void renderItem
(
    NVGcontext* context,
    int32_t item,
    int corners
)
{
    const OUIHeader* header = reinterpret_cast<const OUIHeader*>(uiGetHandle(item));
    UIrect rect = uiGetRect(item);
    if (header)
    {
        switch (header->itemType)
        {
        case OUIItemType::frame:
            {
                auto data = reinterpret_cast<const OUIFrame*>(header);
                if (data->renderCb)
                {
                    data->renderCb(data->callbackContext, context);
                }
                renderItems(context, item);
            }
            break;
        case OUIItemType::column:
            renderItemsVertical(context, item);
            break;
        case OUIItemType::button:
            {
                auto data = reinterpret_cast<const OUIButtonData*>(header);
                bndToolButton(context, rect.x, rect.y, rect.w, rect.h, corners,
                    (BNDwidgetState)uiGetState(item), data->iconId, data->label);
            }
            break;
        case OUIItemType::listbox:
            renderListbox(reinterpret_cast<const OUIListBoxData*>(header),
                context, rect, item, corners);
            break;
        default:
            break;
        }
    }
    
}

static void renderItems
(
    NVGcontext* context,
    int parent
)
{
    for (auto id = uiFirstChild(parent); id > 0; id = uiNextSibling(id))
    {
        renderItem(context, id, UITHEME_CORNER_NONE);
    }
}

static void renderItemsVertical
(
    NVGcontext* context,
    int parent
)
{
    auto id = uiFirstChild(parent);
    if (id < 0)
        return;

    auto nextid = uiNextSibling(id);
    if (nextid < 0)
    {
        renderItem(context, id, UITHEME_CORNER_NONE);
        return;
    }
    else
    {
        renderItem(context, id, UITHEME_CORNER_TOP);
    }
    
    id = nextid;
    while ((nextid = uiNextSibling(id)) > 0)
    {
        renderItem(context, id, UITHEME_CORNER_ALL);
        id = nextid;
    }
    
    renderItem(context, id, UITHEME_CORNER_BOTTOM);
}
/*
void drawLines(struct NVGcontext* vg, float x, float y, float w, float h, float t)
{
	int i, j;
	float pad = 5.0f, s = w/9.0f - pad*2;
	float pts[4*2], fx, fy;
	int joins[3] = {NVG_MITER, NVG_ROUND, NVG_BEVEL};
	int caps[3] = {NVG_BUTT, NVG_ROUND, NVG_SQUARE};
	NVG_NOTUSED(h);

	nvgSave(vg);
	pts[0] = -s*0.25f + cosf(t*0.3f) * s*0.5f;
	pts[1] = sinf(t*0.3f) * s*0.5f;
	pts[2] = -s*0.25f;
	pts[3] = 0;
	pts[4] = s*0.25f;
	pts[5] = 0;
	pts[6] = s*0.25f + cosf(-t*0.3f) * s*0.5f;
	pts[7] = sinf(-t*0.3f) * s*0.5f;

	for (i = 0; i < 1; i++)
	{
		for (j = 0; j < 1; j++)
		{
			fx = x + s*0.5f + (i*3+j)/9.0f*w + pad;
			fy = y - s*0.5f + pad;

			nvgLineCap(vg, caps[i]);
			nvgLineJoin(vg, joins[j]);

			nvgStrokeWidth(vg, s*0.3f);
			nvgStrokeColor(vg, nvgRGBA(0,0,0,160));
			nvgBeginPath(vg);
			nvgMoveTo(vg, fx+pts[0], fy+pts[1]);
			nvgLineTo(vg, fx+pts[2], fy+pts[3]);
			nvgLineTo(vg, fx+pts[4], fy+pts[5]);
			nvgLineTo(vg, fx+pts[6], fy+pts[7]);
			nvgStroke(vg);

			nvgLineCap(vg, NVG_BUTT);
			nvgLineJoin(vg, NVG_BEVEL);

			nvgStrokeWidth(vg, 1.0f);
			nvgStrokeColor(vg, nvgRGBA(0,192,255,255));
			nvgBeginPath(vg);
			nvgMoveTo(vg, fx+pts[0], fy+pts[1]);
			nvgLineTo(vg, fx+pts[2], fy+pts[3]);
			nvgLineTo(vg, fx+pts[4], fy+pts[5]);
			nvgLineTo(vg, fx+pts[6], fy+pts[7]);
			nvgStroke(vg);
		}
	}

	nvgRestore(vg);
}
*/

    
void render(NVGcontext* context, const gfx::Rect& viewRect)
{
    //  pixel ratio may change if for some reason we have different framebuffer
    //  sizes from our window (i.e. fullscreen?)
    nvgBeginFrame(context, viewRect.w, viewRect.h, 1.0f);
    
    //drawLines(context, 50, viewRect.h-100, 600, 40, 0);
    
    //bndBackground(context, 0,0 , viewRect.w, viewRect.h);
    
    if (uiGetItemCount() > 0)
    {
        bndSetFont(nvgFindFont(context, "system"));
        renderItem(context, 0, UITHEME_CORNER_NONE);
    }
    
    nvgEndFrame(context);
}

} /* namespace uicore */ } /* namespace cinek */
