//
//  ComponentData.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 4/19/16.
//
//

#include "ComponentData.hpp"

#include <cstring>
#include <type_traits>

namespace cinek {
    namespace ove {
    
ComponentData::ComponentData() :
    _type(Type::kNull)
{
    //  all types are POD or trivial, so memcpy works on copy/move
    //  until there's a type that doesn't match the POD or trivial requirement
    static_assert(std::is_trivial<ckm::vector3>(), "Point3 must by trivial");
    static_assert(std::is_trivial<ckm::quat>(), "Quaternion must be trivial");
    static_assert(std::is_trivial<ComponentEnumHandle>(), "Enum must be trivial");
}

ComponentData::ComponentData(const ComponentData& data) :
    _type(data._type)
{
    memcpy(_buf, data._buf, sizeof(_buf));
}

ComponentData::ComponentData(ComponentData&& data) :
    _type(data._type)
{
    data._type = Type::kNull;
    memmove(_buf, data._buf, sizeof(_buf));
}

ComponentData& ComponentData::operator=(const ComponentData& data)
{
    _type = data._type;
    memcpy(_buf, data._buf, sizeof(_buf));
    return *this;
}

ComponentData& ComponentData::operator=(ComponentData&& data)
{
    _type = data._type;
    data._type = Type::kNull;
    memmove(_buf, data._buf, sizeof(_buf));
    return *this;
}

void ComponentData::clear()
{
    //  used for destructing data based on type
    //  this is only useful if we have a non-trivial type
    //  as of this version, there are no non-trival types, so it's a stub
    _type = Type::kNull;
}

void ComponentData::set(const char* v)
{
    clear();
    _type = Type::kString;
    *reinterpret_cast<const char**>(&_buf[0]) = v;
}

void ComponentData::set(const ckm::scalar v)
{
    clear();
    _type = Type::kNumber;
    *reinterpret_cast<ckm::scalar*>(&_buf[0]) = v;
}

void ComponentData::set(const int32_t v)
{
    clear();
    _type = Type::kInt32;
    *reinterpret_cast<int32_t*>(&_buf[0]) = v;
}

void ComponentData::set(const uint32_t v)
{
    clear();
    _type = Type::kUInt32;
    *reinterpret_cast<uint32_t*>(&_buf[0]) = v;
}

void ComponentData::set(const ckm::vector3 v)
{
    clear();
    _type = Type::kPoint3;
    ::new(_buf) ckm::vector3(v);
}

void ComponentData:: set(const ckm::quat v)
{
    clear();
    _type = Type::kQuaternion;
    ::new(_buf) ckm::quat(v);
}

void ComponentData::set(const ComponentEnumHandle v)
{
    clear();
    _type = Type::kEnum;
    ::new(_buf) ComponentEnumHandle(v);
}

template<> const char* ComponentData::get<const char*>() const
{
    CK_ASSERT_RETURN_VALUE(_type == Type::kString, nullptr);
    return reinterpret_cast<const char*>(&_buf[0]);
}

template<> ckm::scalar ComponentData::get<ckm::scalar>() const
{
    CK_ASSERT_RETURN_VALUE(_type == Type::kNumber, NAN);
    return *reinterpret_cast<const ckm::scalar*>(&_buf[0]);
}

template<> int32_t ComponentData::get<int32_t>() const
{
    CK_ASSERT_RETURN_VALUE(_type == Type::kInt32, 0);
    return *reinterpret_cast<const int32_t*>(&_buf[0]);
}

template<> uint32_t ComponentData::get<uint32_t>() const
{
    CK_ASSERT_RETURN_VALUE(_type == Type::kUInt32, 0U);
    return *reinterpret_cast<const uint32_t*>(&_buf[0]);
}

template<> ckm::vector3 ComponentData::get<ckm::vector3>() const
{
    CK_ASSERT_RETURN_VALUE(_type == Type::kPoint3, ckm::vector3());
    return *reinterpret_cast<const ckm::vector3*>(&_buf[0]);
}

template<> ckm::quat ComponentData::get<ckm::quat>() const
{
    CK_ASSERT_RETURN_VALUE(_type == Type::kQuaternion, ckm::quat());
    return *reinterpret_cast<const ckm::quat*>(&_buf[0]);
}

template<> ComponentEnumHandle ComponentData::get<ComponentEnumHandle>() const
{
    CK_ASSERT_RETURN_VALUE(_type == Type::kEnum, ComponentEnumHandle());
    return *reinterpret_cast<const ComponentEnumHandle*>(&_buf[0]);
}



    
    } /* namespace ove */
} /* namespace cinek */
