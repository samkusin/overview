//
//  UIService.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 3/21/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#ifndef UIService_hpp
#define UIService_hpp

#include "UITypes.hpp"
#include "UIBuilder.hpp"

namespace cinek {

class UIService
{
public:
    explicit UIService(uicore::Context* ctx);
    
    class Frame
    {
    public:
        Frame(uicore::FrameState* frameState);
        int width() const;
        int height() const;
        int item() const;
        
        UIevent eventType() const;
        uicore::KeyEvent popKey();

    private:
        uicore::FrameState* _frameState;
    };
    
    Frame frame() const;
    
    void setKeyboardFocusToItem(int item);
    
    uicore::Layout createLayout() const;
    
private:
    uicore::Context* _context;
};

}

#endif /* UIService_hpp */
