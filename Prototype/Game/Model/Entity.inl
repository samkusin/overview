///
/// @file
/// Inline Definition for the Entity
///
/// @author     Samir Sinha
/// @date       09/22/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

namespace cinekine {
    namespace ovengine {

    inline EntityId Entity::id() const {
        return _id;
    }
        
    inline const EntityTemplate& Entity::sourceTemplate() const {
        return _template;
    }

    }   /* namespace ovengine */
}   /* namespace cinekine */
