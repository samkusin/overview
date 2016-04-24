//
//  ComponentData.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 4/19/16.
//
//

#ifndef Overview_Component_Data_hpp
#define Overview_Component_Data_hpp

#include "EngineTypes.hpp"

#include <ckm/mathtypes.hpp>
#include <string>

#include "Debug.hpp"

#define OVERVIEW_COMPONENT_PROPERTY(_name_, _type_, _longname_) \
    constexpr static const char k##_name_##Label[] = _longname_; \
    void set##_name_(_type_); \
    _type_ get##_name_() const;


#define OVERVIEW_COMPONENT_PROPERTY_IMPL(_class_, _name_, _type_) \
    constexpr const char _class_::k##_name_##Label[];

#define OVERVIEW_COMPONENT_PROPERTY_STRING(_name_, _longname_) \
    OVERVIEW_COMPONENT_PROPERTY(_name_, const char*, _longname_)
    
#define OVERVIEW_COMPONENT_PROPERTY_NUMBER(_name_, _longname_) \
    OVERVIEW_COMPONENT_PROPERTY(_name_, ckm::scalar, _longname_)
    
#define OVERVIEW_COMPONENT_PROPERTY_INT32(_name_, _longname_) \
    OVERVIEW_COMPONENT_PROPERTY(_name_, int32_t, _longname_)

#define OVERVIEW_COMPONENT_PROPERTY_UINT32(_name_, _longname_) \
    OVERVIEW_COMPONENT_PROPERTY(_name_, uint32_t, _longname_)

#define OVERVIEW_COMPONENT_PROPERTY_POINT3(_name_, _longname_) \
    OVERVIEW_COMPONENT_PROPERTY(_name_, ckm::vector3, _longname_)

#define OVERVIEW_COMPONENT_PROPERTY_POINT3_IMPL(_class_, _name_) \
    OVERVIEW_COMPONENT_PROPERTY_IMPL(_class_, _name_, ckm::vector3)

#define OVERVIEW_COMPONENT_PROPERTY_QUATERNION(_name_, _longname_) \
    OVERVIEW_COMPONENT_PROPERTY(_name_, ckm::quat, _longname_)

#define OVERVIEW_COMPONENT_PROPERTY_ENUM(_name_, _longname_) \
    OVERVIEW_COMPONENT_PROPERTY(_name_, ComponentData::EnumHandle, _longname_)


namespace cinek {
    namespace ove {
    
struct ComponentEnumHandle
{
    int enumType;
    int enumValue;
    ComponentEnumHandle() = default;
    ComponentEnumHandle(int type, int value) : enumType(type), enumValue(value) {}
    explicit operator bool() const { return enumType >= 0; }
};
    
class ComponentData
{
public:
    enum class Type
    {
        kNull,
        kString,
        kNumber,
        kInt32,
        kUInt32,
        kPoint3,
        kQuaternion,
        kEnum
    };
 
    ComponentData();
    ComponentData(const ComponentData& data);
    ComponentData(ComponentData&& data);
    template<typename T> ComponentData(const T data);
    
    ComponentData& operator=(const ComponentData& data);
    ComponentData& operator=(ComponentData&& data);
    
    explicit operator bool() const { return _type != Type::kNull; }
    Type type() const { return _type; }
    
    template<typename T> T get() const;
    
    void set(const char* v);
    void set(const ckm::scalar v);
    void set(const int32_t v);
    void set(const uint32_t v);
    void set(const ckm::vector3 v);
    void set(const ckm::quat v);
    void set(const ComponentEnumHandle v);
    
    void clear();
    
private:
    static const int kBufferSize = sizeof_max<ckm::quat, ckm::vector3>::size;
    Type _type;
    char _buf[kBufferSize];
};

#include "ComponentData.inl"
    
    } /* namespace ove */
} /* namespace cinek */


#endif /* ComponentData_hpp */
