//
//  StartupView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#ifndef Sample_Engine_StartupView_hpp
#define Sample_Engine_StartupView_hpp

#include "Engine/ViewController.hpp"
#include "Engine/EngineTypes.hpp"

namespace cinek {
    namespace ove {


class StartupView : public ViewController
{
public:
    StartupView(ViewAPI& viewAPI);
    
    virtual void onViewLoad();
    virtual void onViewUnload();
    virtual void onViewAdded();
    virtual void onViewRemoved();
    virtual void onViewForeground();
    virtual void onViewBackground();
    virtual void simulateView(double time, double dt);
    virtual void layoutView();
    virtual void frameUpdateView(double dt);
    virtual const char* viewId() const;
    
private:
    ViewAPI* _viewAPI;
};


    } /* namespace ove */
} /* namespace cinek */

#endif /* Sample_Engine_StartupView_hpp */
