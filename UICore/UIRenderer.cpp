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
#include <ckm/math.hpp>

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

static void renderListbox
(
    OUIListBoxData* lbcontext,
    NVGcontext* nvg,
    const UIrect& rect,
    int item,
    int corners
)
{
    const float kCornerRadius = 3.0f;
    const int kBoxMarginLR = 5;
    const int kBoxMarginTB = 10;
    
    UIvec2 mouse = uiGetCursor();
    
    //  start
    nvgSave(nvg);

    /*
    //  drop shadow
    struct NVGpaint shadow = nvgBoxGradient(nvg, rect.x, rect.y+4, rect.w, rect.h,
        kCornerRadius*2, 10,
        nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
    nvgBeginPath(nvg);
    nvgRect(nvg, rect.x-10, rect.y-10, rect.w+20, rect.h+30);
    nvgRoundedRect(nvg, rect.x, rect.y, rect.w, rect.h, kCornerRadius);
    nvgPathWinding(nvg, NVG_HOLE);
    nvgFillPaint(nvg, shadow);
    nvgFill(nvg);
    */
    //  draw the box window
    nvgBeginPath(nvg);
    nvgRoundedRect(nvg, rect.x, rect.y, rect.w, rect.h, kCornerRadius);
    nvgFillColor(nvg, nvgRGBA(76, 76, 76, 255));
    nvgFill(nvg);
    
    //  content
    //  - we're rendering the entire listbox and using a scissor to clip
    //    contents to a frame within our window.  this is the simplest way to
    //    handle listboxes.  this method should suffice for most listboxes.  if
    //    we have a case of listboxes with an 'infinite' (dynamically generated)
    //    list of contents, we'll have to employ another method
    
    if (lbcontext->provider) {
        int numItems = lbcontext->provider->onUIDataItemRowCountRequest(lbcontext->providerId);
        
        int lmargin = rect.x+kBoxMarginLR;
        int rmargin = lmargin + rect.w-2*kBoxMarginLR;
        
        if (numItems > 0 && lmargin < rmargin) {
            nvgSave(nvg);
            nvgFontSize(nvg, BND_LABEL_FONT_SIZE);
            nvgFontFaceId(nvg, bnd_font);
            nvgTextAlign(nvg, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
            
            nvgScissor(nvg, rect.x, rect.y, rect.w, rect.h);
        
            float cellWidth;
            float cellHeight;
            
            Box cellMargins;         // trbl
            
            if (lbcontext->lbtype == ListboxType::kList) {
                cellMargins.t = 1;
                cellMargins.r = 2;
                cellMargins.b = 1;
                cellMargins.l = 2;
                cellWidth = rect.w - cellMargins.l - cellMargins.r;
                cellHeight = 0;
            }
            else {
                cellMargins.t = cellMargins.r = cellMargins.b = cellMargins.l = 0;
                cellWidth = 0;
                cellHeight = 0;
                CK_ASSERT(false);   // unsupported type?  bad things may happen
            }
            
            UIvec2 cellOrigin { lmargin, rect.y + kBoxMarginTB };
        
            uint32_t row = 0;
            DataObject data;
            while (lbcontext->provider->onUIDataItemRequest(lbcontext->providerId, row, 0, data)) {
                
                UIvec2 rowDim = { 0, 1 };
                
                if (lbcontext->lbtype == ListboxType::kList) {
                    //  listbox cells have a fixed height, determined by the first
                    //  row's height.  width will be the width of the longest row
                    //
                    
                    //  visual style is modelled against Blender's file dialog list
                    
                    float offsetX = cellOrigin.x + cellMargins.l;
                    float offsetY = cellOrigin.y + cellMargins.t;
        
                    //  attempt to fill the row with content
                    if (data.type == DataObject::Type::string) {
                        //float width = nvgTextBounds(nvg, 0, 0, data.data.str, nullptr, nullptr);
                        
                        if (cellHeight < ckm::kEpsilon) {
                            nvgTextMetrics(nvg, NULL, NULL, &cellHeight);
                        }
                        
                        rowDim.x = cellMargins.l + cellWidth + cellMargins.r;
                        rowDim.y = cellMargins.t + cellHeight + cellMargins.b;
                        
                        if (row == lbcontext->state->highlightItem) {
                            nvgBeginPath(nvg);
                            nvgFillColor(nvg, nvgRGBA(191, 116, 40, 255));
                            nvgRoundedRect(nvg, cellOrigin.x, cellOrigin.y,
                                rmargin-lmargin, cellMargins.t + cellHeight + cellMargins.b,
                                kCornerRadius);
                            nvgFill(nvg);
                        }
                        
                        if (mouse.y < cellOrigin.y + rowDim.y && mouse.y >= cellOrigin.y &&
                            mouse.x < cellOrigin.x + rowDim.x && mouse.x >= cellOrigin.x) {
                            if (row != lbcontext->state->highlightItem) {
                                nvgBeginPath(nvg);
                                nvgFillColor(nvg, nvgRGBA(96, 96, 96, 255));
                                nvgRoundedRect(nvg, cellOrigin.x, cellOrigin.y,
                                    rmargin-lmargin, cellMargins.t + cellHeight + cellMargins.b,
                                    kCornerRadius);
                                nvgFill(nvg);
                            }
                            lbcontext->state->hoverItem = (int)row;
                        }
                        
                        //printf("%d,%d\n", mouse.x, mouse.y);
                        
                        nvgFillColor(nvg, BND_COLOR_TEXT_SELECTED);
                        nvgText(nvg, offsetX, offsetY, data.data.str, nullptr);
                    }

                    
                }
                
                cellOrigin.y += rowDim.y;
                
                ++row;
            }
           
            nvgRestore(nvg);
        }
    }
    
    //  finished
    nvgRestore(nvg);
}

static void renderWindow
(
    OUIWindow* data,
    NVGcontext* nvg,
    const UIrect& rect,
    int item
)
{
    const float kCornerRadius = 3.0f;
    
	struct NVGpaint shadowPaint;
	
	nvgSave(nvg);
	//	nvgClearState(vg);
    
    // Window
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, rect.x, rect.y, rect.w, rect.h, kCornerRadius);
	nvgFillColor(nvg, nvgRGBA(28,30,34,192) );
	//	nvgFillColor(vg, nvgRGBA(0,0,0,128) );
	nvgFill(nvg);
    
	// Drop shadow
	shadowPaint = nvgBoxGradient(nvg, rect.x, rect.y+2, rect.w, rect.h,
        kCornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0) );
	nvgBeginPath(nvg);
	nvgRect(nvg, rect.x-10, rect.y-10, rect.w+20, rect.h+30);
	nvgRoundedRect(nvg, rect.x, rect.y, rect.w, rect.h, kCornerRadius);
	nvgPathWinding(nvg, NVG_HOLE);
	nvgFillPaint(nvg, shadowPaint);
	nvgFill(nvg);

    // Title Bar
    if (data->title && *data->title) {
        nvgBeginPath(nvg);
        nvgRoundedRect(nvg, rect.x, rect.y, rect.w, UITHEME_WIDGET_HEIGHT, kCornerRadius);
        nvgFillColor(nvg, nvgRGBA(0,0,0,192) );
        nvgFill(nvg);
        
        // Title
         
        nvgFillColor(nvg, BND_COLOR_TEXT);
        nvgText(nvg, 1, 1, data->title, nullptr);
    }

	nvgRestore(nvg);
}


static void renderItems(NVGcontext* context, int parent);
static void renderItemsVertical(NVGcontext* context, int parent);
static void renderItemsHorizontal(NVGcontext* context, int parent);

static void renderItem
(
    NVGcontext* context,
    int32_t item,
    int corners
)
{
    OUIHeader* header = reinterpret_cast<OUIHeader*>(uiGetHandle(item));
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
        case OUIItemType::group:
            renderItems(context, item);
            break;
        case OUIItemType::column:
            renderItemsVertical(context, item);
            break;
        case OUIItemType::row:
            renderItemsHorizontal(context, item);
            break;
        case OUIItemType::window:
            {
                renderWindow(reinterpret_cast<OUIWindow*>(header), context, rect, item);
                renderItems(context, item);
            }
            break;
        case OUIItemType::button:
            {
                auto data = reinterpret_cast<const OUIButtonData*>(header);
                bndToolButton(context, rect.x, rect.y, rect.w, rect.h, corners,
                    (BNDwidgetState)uiGetState(item), data->iconId, data->label);
            }
            break;
        case OUIItemType::listbox:
            renderListbox(reinterpret_cast<OUIListBoxData*>(header),
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

static void renderItemsHorizontal
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
        renderItem(context, id, UITHEME_CORNER_LEFT);
    }
    
    id = nextid;
    while ((nextid = uiNextSibling(id)) > 0)
    {
        renderItem(context, id, UITHEME_CORNER_ALL);
        id = nextid;
    }
    
    renderItem(context, id, UITHEME_CORNER_RIGHT);
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
