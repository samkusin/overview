//
//  Component.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 4/21/16.
//
//

template<typename Object>
Component<Object>::Component(Object* object) :
    _object(object)
{
}

template<typename Object>
template<typename Cb>
void Component<Object>::enumerateProperties(Cb cb)
{
    int sz = getPropertyCount();
    for (int i = 0; i < sz; ++i) {
        auto data = getPropertyData(i);
        if (cb(data.first, data.second)) {
            // write data
            writeProperty(data.first, data.second);
        }
    }
}

template<typename Object>
int Component<Object>::getPropertyCount() const
{
    return sPropertyCount;
}

template<typename Object>
std::pair<const char*, ComponentData> Component<Object>::getPropertyData(int index) const
{
    std::pair<const char*, ComponentData> data;
    data.first = nullptr;
    
    CK_ASSERT_RETURN_VALUE(index >= 0 && index < sPropertyCount, data);
    data.first = sPropertyNames[index];
    readProperty(data.first, data.second);
    
    return data;
}
