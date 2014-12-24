///
/// @file
/// Inline Definition for the Entity
///
/// @author     Samir Sinha
/// @date       09/22/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

namespace cinek {
    namespace overview {

    inline EntityId Entity::id() const {
        return _id;
    }
        
    inline bool Entity::operator<(const Entity& rhs) const {
        return _id < rhs._id;
    }
        
    inline const EntityTemplate& Entity::sourceTemplate() const
    {
        return _template;
    }

    inline uint32_t Entity::resultFlags() const
    {
        return _updateFlags;
    }
    
    inline void Entity::resetResultFlags()
    {
        _updateFlags = 0;
    }
    
    inline void Entity::setResult(uint32_t resultFlag, bool state)
    {
        if (state)
        {
            _updateFlags |= resultFlag;
        }
        else
        {
            _updateFlags &= ~(resultFlag);
        }
    }

    }   /* namespace overview */
}   /* namespace cinek */
