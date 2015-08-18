///
/// @file
/// Header documentation for an engine component
///
/// @author     Samir Sinha
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Engine_hpp
#define Overview_Engine_hpp

#include "Engine/EngineMathTypes.hpp"

#include <cinek/types.hpp>


namespace cinek { namespace overview {

class ViewStack;

//  The Entity is the one of the few engine-wide types.  EntityTypes.hpp
//  contains prototypes for other Entity related objects.

struct Entity
{
    using value_type = uint64_t;
    using index_type = uint32_t;
    using iteration_type = uint16_t;
    using context_type = uint16_t;
    static constexpr value_type null_value = 0;
    static Entity null()  { Entity eid; eid.id = 0; return eid; }
    
    void makeEntityId(iteration_type iteration, context_type context, index_type index) {
        id = iteration;
        id = (id << 16) | context;
        id = (id << 32) | index;
    }
    
    bool operator==(std::nullptr_t) const {
        return id == null_value;
    }
    bool operator!=(std::nullptr_t) const {
        return id != null_value;
    }
    bool operator==(const Entity& other) const {
        return id == other.id;
    }
    bool operator!=(const Entity& other) const {
        return id != other.id;
    }
    
    Entity& operator=(std::nullptr_t)
    {
        id = null_value;
        return *this;
    }
    bool valid() const { return id != null_value; }
    
    value_type value() const { return id; }
    index_type index() const { return ((index_type)id & 0xffffffff); }
    context_type context() const { return ((context_type)(id >> 32)); }
    iteration_type iteration() const { return (iteration_type)(id >> 48); }
    
    value_type id;
};

} /* namespace overview */ } /* namespace cinek */

#endif
