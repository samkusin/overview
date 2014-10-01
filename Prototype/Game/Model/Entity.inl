///
/// @file
/// Inline Definition for the Entity
///
/// @author     Samir Sinha
/// @date       09/22/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

namespace cinekine {
    namespace prototype {

    EntityId Entity::id() const {
        return _id;
    }

    inline const glm::vec3& Entity::position() const {
        return _position;
    }

    }   /* namespace prototype */
}   /* namespace cinekine */
