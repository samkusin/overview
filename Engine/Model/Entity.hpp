///
/// @file
/// Class definition for the Entity
///
/// @author     Samir Sinha
/// @date       09/14/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_Entity_hpp
#define Overview_Model_Entity_hpp

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Model/ViewModelInstance.hpp"


namespace cinekine { namespace ovengine {

/**
 * @class   Entity
 * @brief   An Entity is a simulation specific specialization of various models.
 * @details An application creates Entity objects from one or more models.  The
 *          Entity acts as a collection of these models' instances.  At its
 *          simplest, an Entity may contain a single SpriteInstance.  Or it may
 *          contain a reference to a Tile from a TileGridMap.  Or it may contain
 *          either a Sprite, or a Tile, plus a Physics model, plus an AI model
 *          instance, etc.
 */
class Entity
{
public:
    Entity();
    Entity(const ViewModelInstance& modelInst);

    void setViewModelInstance(const ViewModelInstance& modelInst);
    ViewModelInstance viewModelInstance() const;

private:
    ViewModelInstance _viewInstance;
};

inline ViewModelInstance Entity::viewModelInstance() const
{
    return _viewInstance;
}

} /* namespace ovengine */ } /* namespace cinekine */

#endif
