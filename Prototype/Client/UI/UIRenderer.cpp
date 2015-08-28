//
//  UIRenderer.cpp
//  Overview
//
//  Created by Samir Sinha on 7/20/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include "UIRenderer.hpp"
#include "UIEngine.hpp"

#include "Client/Diagnostics.hpp"
#include "Engine/Entity/EntityStore.hpp"

#include "oui.h"

#include <bgfx/bgfx.h>

#include "CKGfx/External/nanovg/nanovg.h"

#define BLENDISH_IMPLEMENTATION
#include "blendish.h"

#include <ctime>

namespace cinek { namespace ovproto {

NVGcontext* createUIRenderingContext(int viewId)
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

void destroyUIRenderingContext(NVGcontext* nvg)
{
    nvgDelete(nvg);
}


static void renderUIItems(NVGcontext* context, int parent);
static void renderUIItemsVertical(NVGcontext* context, int parent);

static void renderUIItem
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
                renderUIItems(context, item);
            }
            break;
        case OUIItemType::column:
            renderUIItemsVertical(context, item);
            break;
        case OUIItemType::button:
            {
                auto data = reinterpret_cast<const OUIButtonData*>(header);
                bndToolButton(context, rect.x, rect.y, rect.w, rect.h, corners,
                    (BNDwidgetState)uiGetState(item), data->iconId, data->label);
            }
            break;
        default:
            break;
        }
    }
    
}

static void renderUIItems
(
    NVGcontext* context,
    int parent
)
{
    for (auto id = uiFirstChild(parent); id > 0; id = uiNextSibling(id))
    {
        renderUIItem(context, id, UITHEME_CORNER_NONE);
    }
}

static void renderUIItemsVertical
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
        renderUIItem(context, id, UITHEME_CORNER_NONE);
        return;
    }
    else
    {
        renderUIItem(context, id, UITHEME_CORNER_TOP);
    }
    
    id = nextid;
    while ((nextid = uiNextSibling(id)) > 0)
    {
        renderUIItem(context, id, UITHEME_CORNER_ALL);
        id = nextid;
    }
    
    renderUIItem(context, id, UITHEME_CORNER_BOTTOM);
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

    
void renderUI(NVGcontext* context, const gfx::Rect& viewRect)
{
    //  pixel ratio may change if for some reason we have different framebuffer
    //  sizes from our window (i.e. fullscreen?)
    nvgBeginFrame(context, viewRect.w, viewRect.h, 1.0f);
    
    //drawLines(context, 50, viewRect.h-100, 600, 40, 0);
    
    //bndBackground(context, 0,0 , viewRect.w, viewRect.h);
    
    if (uiGetItemCount() > 0)
    {
        bndSetFont(nvgFindFont(context, "system"));
        renderUIItem(context, 0, UITHEME_CORNER_NONE);
    }
    
    nvgEndFrame(context);
}

void renderDiagnostics
(
    Diagnostics& diagnostics,
    overview::EntityDiagnostics& entityDiagnostics,
    NVGcontext* context,
    const gfx::Rect& viewRect
)
{
    nvgBeginFrame(context, viewRect.w, viewRect.h, 1.0f);
    {
        //  draw diagnostics frame
        int left = 0;   //viewRect.w * 0.025f;
        int top = 0;    //viewRect.h * 0.05f;
        int width = viewRect.w * 0.40f;
        int height = viewRect.h * 0.75f;
        int right = left + width;
        int paddingLR = width * 0.025f;
        int paddingTB = height * 0.025f;
        
        nvgBeginPath(context);
        nvgRect(context, left, top, width, height);
        nvgFillColor(context, nvgRGBA(128,0,0,64));
        nvgFill(context);
        
        nvgFontFace(context, "system");
        nvgFontSize(context, 12);
        
        float lineHeight;
        nvgTextMetrics(context, NULL, NULL, &lineHeight);
        
        nvgFillColor(context, nvgRGBA(255,255,255,255));
        
        int leftColX = left + paddingLR;
        int rightColX = left + width * 0.33f;
        int rightColW = right - paddingLR - rightColX;
        int rowY = top + paddingTB + lineHeight;
        int rowHeight = lineHeight + 4;
        
        int columnX_1 = rightColX;
        int columnX_2 = rightColX + rightColW * 0.33f;
        int columnX_3 = rightColX + rightColW * 0.66f;
        
        //  HEADER
        char buf[64];
        auto systime = diagnostics.currentTimeInMs()/1000;
        struct ::tm ftime;
        ftime.tm_hour = systime / 3600;
        ftime.tm_min = (systime % 3600) / 60;
        ftime.tm_sec = (systime % 3600) % 60;
        strftime(buf, sizeof(buf)-1, "System: %H:%M:%S", &ftime);
        nvgText(context, leftColX, rowY, buf, nullptr);
        
        systime = diagnostics.simTimeInMs()/1000;
        ftime.tm_hour = systime / 3600;
        ftime.tm_min = (systime % 3600) / 60;
        ftime.tm_sec = (systime % 3600) % 60;
        strftime(buf, sizeof(buf)-1, "Sim: %H:%M:%S", &ftime);
        nvgText(context, rightColX, rowY, buf, nullptr);
        
        rowY += rowHeight;
        
        nvgText(context, leftColX, rowY, "Rate", nullptr);
        nvgText(context, columnX_1, rowY, "1s", nullptr);
        nvgText(context, columnX_2, rowY, "15s", nullptr);
        nvgText(context, columnX_3, rowY, "60s", nullptr);
        
        rowY += rowHeight;

        nvgBeginPath(context);
        nvgStrokeColor(context, nvgRGBA(192, 192, 192, 255));
        nvgMoveTo(context, leftColX, rowY);
        nvgLineTo(context, rightColX + rightColW, rowY);
        nvgStroke(context);
        
        rowY += rowHeight;
        
        //  STATS
        nvgText(context, leftColX, rowY, "Render FPS:", nullptr);

        snprintf(buf, sizeof(buf), "%.1f",
            diagnostics.rateGaugeAmount(Diagnostics::kFrameRate_Main, Diagnostics::kInterval_Second));
        nvgText(context, columnX_1, rowY, buf, nullptr);
        
        snprintf(buf, sizeof(buf), "%.1f",
            diagnostics.rateGaugeAmount(Diagnostics::kFrameRate_Main, Diagnostics::kInterval_QuarterMinute));
        nvgText(context, columnX_2, rowY, buf, nullptr);
        
        snprintf(buf, sizeof(buf), "%.1f",
            diagnostics.rateGaugeAmount(Diagnostics::kFrameRate_Main, Diagnostics::kInterval_Minute));
        nvgText(context, columnX_3, rowY, buf, nullptr);
        
        rowY += rowHeight;
        
        //  STATS
        nvgText(context, leftColX, rowY, "Update FPS:", nullptr);

        snprintf(buf, sizeof(buf), "%.1f",
            diagnostics.rateGaugeAmount(Diagnostics::kFrameRate_Update, Diagnostics::kInterval_Second));
        nvgText(context, columnX_1, rowY, buf, nullptr);
        
        snprintf(buf, sizeof(buf), "%.1f",
            diagnostics.rateGaugeAmount(Diagnostics::kFrameRate_Update, Diagnostics::kInterval_QuarterMinute));
        nvgText(context, columnX_2, rowY, buf, nullptr);
        
        snprintf(buf, sizeof(buf), "%.1f",
            diagnostics.rateGaugeAmount(Diagnostics::kFrameRate_Update, Diagnostics::kInterval_Minute));
        nvgText(context, columnX_3, rowY, buf, nullptr);
        
        rowY += rowHeight*2;
        
        //  ENTITY STATS
        
        nvgText(context, leftColX, rowY, "Entities", nullptr);
        snprintf(buf, sizeof(buf), "Cnt: %u", entityDiagnostics.entityCount);
        nvgText(context, columnX_1, rowY, buf, nullptr);
        snprintf(buf, sizeof(buf), "Max: %u", entityDiagnostics.entityLimit);
        nvgText(context, columnX_2, rowY, buf, nullptr);
        
        rowY += rowHeight;
        
        nvgText(context, leftColX, rowY, "Component", nullptr);
        nvgText(context, columnX_1, rowY, "Allocated", nullptr);
        nvgText(context, columnX_2, rowY, "Limit", nullptr);
        
        rowY += rowHeight;

        nvgBeginPath(context);
        nvgStrokeColor(context, nvgRGBA(192, 192, 192, 255));
        nvgMoveTo(context, leftColX, rowY);
        nvgLineTo(context, rightColX + rightColW, rowY);
        nvgStroke(context);
        
        rowY += rowHeight;
        
        for (auto& component : entityDiagnostics.components)
        {
            nvgText(context, leftColX, rowY, component.name, nullptr);
            
            snprintf(buf, sizeof(buf), "%u", component.allocated);
            nvgText(context, columnX_1, rowY, buf, nullptr);
            snprintf(buf, sizeof(buf), "%u", component.limit);
            nvgText(context, columnX_2, rowY, buf, nullptr);
            
            rowY += rowHeight;
        }
        
        rowY += rowHeight;
        
        nvgText(context, leftColX, rowY, "Entity Group", nullptr);
        nvgText(context, columnX_1, rowY, "Allocated", nullptr);
        nvgText(context, columnX_2, rowY, "Limit", nullptr);
        
        rowY += rowHeight;

        nvgBeginPath(context);
        nvgStrokeColor(context, nvgRGBA(192, 192, 192, 255));
        nvgMoveTo(context, leftColX, rowY);
        nvgLineTo(context, rightColX + rightColW, rowY);
        nvgStroke(context);
        
        rowY += rowHeight;
        
        for (auto& group : entityDiagnostics.groups)
        {
            snprintf(buf, sizeof(buf), "%u", group.id);
            nvgText(context, leftColX, rowY, buf, nullptr);
            
            snprintf(buf, sizeof(buf), "%u", group.allocated);
            nvgText(context, columnX_1, rowY, buf, nullptr);
            
            snprintf(buf, sizeof(buf), "%u", group.limit);
            nvgText(context, columnX_2, rowY, buf, nullptr);
            
            rowY += rowHeight;
        }
        
        rowY += rowHeight;
    
    }
    nvgEndFrame(context);
}


} /* namespace ovproto */ } /* namespace cinek */
