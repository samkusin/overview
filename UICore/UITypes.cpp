//
//  UITypes.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 2/18/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "UITypes.hpp"

namespace cinek {

UIService::UIService(UIContext* context) :
    _context(context)
{
}

void UIService::setKeyboardFocusToItem(int item)
{
    _context->keyFocusItem = item;
}

}