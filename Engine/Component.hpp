//
//  Component.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 4/19/16.
//
//

#ifndef Overview_Component_hpp
#define Overview_Component_hpp

#include "ComponentData.hpp"
#include <utility>

namespace cinek {
    namespace ove {
    
template<typename Object>
class Component
{
public:
    Component(Object* object);
    
    /**
     *  @return The associated entity for this component
     */
    Entity getEntity() const;
    /**
     *  Enumerates all properties of the component.  Allows modification of data
     *  during enumeration if callback returns true.
     *  
     *  @param cb   The callback issued per property
     */
    template<typename Cb> void enumerateProperties(Cb cb);
    /**
     *  Read property data
     *
     *  @param  name Property name
     *  @param  data Property data to be filled in
     *  @return True if the property exists, or False if it doesn't.
     */
    bool readProperty(const char* name, ComponentData& data) const;
    /**
     *  Write property data.  Note that string data is copied from the supplied
     *  buffer pointer.
     *
     *  @param  name Property name
     *  @param  data Property data to be written.
     *  @return True if the property exists, or False if it doesn't.
     */
    bool writeProperty(const char* name, const ComponentData& data);
    
protected:
    int getPropertyCount() const;
    
    std::pair<const char*, ComponentData> getPropertyData(int index) const;
    
private:
    Object* _object;
    static const char* sPropertyNames[];
    static const int sPropertyCount;
};

#include "Component.inl"
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* Overview_Component_hpp */
