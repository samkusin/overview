//
//  StageApplication.hpp
//  Overview
//
//  Created by Samir Sinha on 5/22/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#ifndef Overview_App_Stage_Application_hpp
#define Overview_App_Stage_Application_hpp

#include "Stage.hpp"

#include "Engine/EngineContext.hpp"
#include "Engine/Render/RenderTypes.hpp"

namespace cinek { namespace ovproto {

class StageApplication : public overview::EngineContext
{
    CK_CLASS_NON_COPYABLE(StageApplication);
    
public:
    StageApplication(
        overview::EntityDatabase& _entityDb,
        overview::RenderResources& renderResources,
        const Allocator& allocator);
    
    void update(uint32_t deltaMs);
    
    void render();
    
private:
    Allocator _allocator;
    
    unique_ptr<Stage> _stage;
    unique_ptr<overview::RenderScene> _scene;
};

} /* namespace ovproto */ } /* namespace cinek */


#endif /* defined(Overview_App_Stage_Application_hpp) */
