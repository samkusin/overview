//
//  ComponentData.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 4/21/16.
//
//

template<typename T> ComponentData::ComponentData(const T data)
{
    set(data);
}

template<typename T> T ComponentData::get() const
{
    //static_assert(false, "Unimplemented data retriever");
    return T();
}