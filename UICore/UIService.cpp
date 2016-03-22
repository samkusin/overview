//
//  UIService.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 3/21/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "UIService.hpp"

#include "UIEngine.hpp"

#include "oui.h"

namespace cinek {

UIService::Frame::Frame(uicore::FrameState* frameState) :
    _frameState(frameState)
{
}

int UIService::Frame::width() const
{
    return uiGetWidth(_frameState->thisItem);
}

int UIService::Frame::height() const
{
    return uiGetHeight(_frameState->thisItem);
}

int UIService::Frame::item() const
{
    return _frameState->thisItem;
}

UIevent UIService::Frame::eventType() const
{
    return _frameState->evtType;
}

uicore::KeyEvent UIService::Frame::popKey()
{
    return _frameState->popKey();
}

 
UIService::UIService(uicore::Context* ctx) :
    _context(ctx)
{
}

UIService::Frame UIService::frame() const
{
    return Frame(&_context->frameState);
}

void UIService::setKeyboardFocusToItem(int item)
{
    _context->keyFocusItem = item;
}

uicore::Layout UIService::createLayout() const
{
    return uicore::Layout(_context);
}

}