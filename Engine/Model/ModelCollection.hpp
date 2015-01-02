///
/// @file
/// The Collection template class header
///
/// @author     Samir Sinha
/// @date       07/16/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_Collection_hpp
#define Overview_Model_Collection_hpp

#include "Engine/Model/ModelTypes.hpp"

#include "cinek/string.hpp"
#include "cinek/vector.hpp"

namespace cinek { namespace overview {

/// @class  ModelCollection
/// @ingroup Model
/// @brief  A group of Model objects sharing common atttributes
///
template<class _Model>
class ModelCollection
{
public:
    /// Defines the collection item type
    typedef _Model          Type;
    typedef vector<_Model>  Container;

public:
    /// Constructor
    ModelCollection() = default;
    /// Constructor
    /// @param  name        The name of the collection
    /// @param  tiles       The tiles defined for this collection
    ///
    ModelCollection(const char* name, Container&& models);
    /// @return True if an empty collection
    ///
    bool empty() const;
    /// @return The number of tiles in the collection
    ///
    size_t count() const;
    /// @return The collection name
    ///
    const std::string& name() const;
    /// Reference a Model in the collection by index
    /// @param  index The Model's index within the collection
    /// @return The Model object
    ///
    Type& operator[](size_t index);
    /// Reference a const Model in the collection by index
    /// @param  index The Model's index within the collection
    /// @return The Model object
    ///
    const Type& operator[](size_t index) const;

protected:
    Container _models;

private:
    std::string _name;
};

////////////////////////////////////////////////////////////////////

template<class _Model>
ModelCollection<_Model>::ModelCollection(const char* name, Container&& models) :
    _models(std::move(models)),
    _name(name)
{

}

template<class _Model>
size_t ModelCollection<_Model>::count() const
{
    return _models.size();
}

template<class _Model>
bool ModelCollection<_Model>::empty() const
{
    return _models.empty();
}

template<class _Model>
const std::string& ModelCollection<_Model>::name() const
{
    return _name;
}

template<class _Model>
auto ModelCollection<_Model>::operator[](size_t index) -> Type&
{
    return _models[index];
}

template<class _Model>
auto ModelCollection<_Model>::operator[](size_t index) const -> const Type&
{
    return _models[index];
}

} /* namespace overview */ } /* namespace cinek */

#endif
