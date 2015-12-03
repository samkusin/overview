//
//  GameView.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/10/15.
//
//

#ifndef Sample_Engine_GameView_hpp
#define Sample_Engine_GameView_hpp

#include "GameTypes.hpp"
#include "Engine/ViewController.hpp"


namespace cinek {
    namespace ove {


class GameView : public ViewController
{
public:
    GameView(GameViewAPI& viewAPI);
    
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
    GameViewAPI* _viewAPI;
};


    } /* namespace ove */
} /* namespace cinek */

#endif /* Sample_Engine_GameView_hpp */
