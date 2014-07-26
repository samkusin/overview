///
/// @file
/// A Model Library represented by multiple ModelCollection objects
///
/// @author     Samir Sinha
/// @date       07/16/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_ModelLibrary_hpp
#define Overview_Model_ModelLibrary_hpp

#include "Engine/Model/Tile.hpp"
#include "Engine/Model/TileCollection.hpp"
#include "Engine/Debug.hpp"

#include "cinek/vector.hpp"

namespace cinekine { namespace ovengine {

/// @class  ModelLibrary
/// @ingroup Model
/// @brief  A container for ModelCollection records.
///
template<class _Collection, class _SlotType>
class ModelLibrary
{
public:
    /// Type ModelCollection type for this library
    typedef _Collection         CollectionType;
    /// The Slot index type for used for collections in this library
    typedef _SlotType           SlotType;

public:
    /// Constructor
    ///
    /// @param collectionLimit  The total number of collections to reserve for
    ///                         the library
    /// @param allocator        An optional allocator
    ///
    ModelLibrary(SlotType collectionLimit, const Allocator& allocator=Allocator());
    /// Clears the library of all tile collections
    ///
    void clear();
    /// Maps a collection to the specified slot.
    ///
    /// @param slot         The collection index/slot for the supplied
    ///                     collection object
    /// @param collection   The collection to bind to the specified slot
    ///
    void mapCollectionToSlot(CollectionType&& collection, SlotType slot);
    /// Retrieves the collection at the specified slot.  The operation is O(1).
    ///
    /// @param  slot    The collection slot
    /// @return The mapped ModelCollection
    ///
    const CollectionType& collectionAtSlot(SlotType slot) const;
    /// Retrieves a collection by name.  The operation is O(n).
    ///
    /// @param  name The collection's name
    /// @return The ModelCollection with that name or an empty collection
    ///
    const CollectionType& collectionByName(const string& name) const;

protected:
    vector<CollectionType> _collections;
    CollectionType _nullCollection;
};

////////////////////////////////////////////////////////////////////////////////

template<class _Collection, class _SlotType>
ModelLibrary<_Collection,_SlotType>::ModelLibrary(SlotType collectionLimit,
                                                  const Allocator& allocator) :
    _collections(collectionLimit, CollectionType(), allocator)
{
}

template<class _Collection, class _SlotType>
void ModelLibrary<_Collection, _SlotType>::clear()
{
    _collections.clear();
}

template<class _Collection, class _SlotType>
void ModelLibrary<_Collection, _SlotType>::mapCollectionToSlot(
                                                CollectionType&& collection,
                                                SlotType slot)
{
    if (slot >= _collections.size())
    {
        OVENGINE_LOG_ERROR("ModelLibrary.mapCollectionToSlot - slot %u exceeds "
                           "the collection limit %u", slot, _collections.size());
        return;
    }
    OVENGINE_LOG_TRACE("ModelLibrary.mapCollectionToSlot - slot %u mapped to "
                       "collection '%s'", slot, collection.name().c_str());

    _collections[slot] = std::move(collection);
}

template<class _Collection, class _SlotType>
auto ModelLibrary<_Collection, _SlotType>::collectionAtSlot(SlotType slot) const
    -> const CollectionType&
{
    return _collections[slot % _collections.size()];
}

template<class _Collection, class _SlotType>
auto ModelLibrary<_Collection, _SlotType>::collectionByName(const string& name) const
    -> const CollectionType&
{
    for (auto& collection: _collections)
    {
        if (collection.name() == name)
            return collection;
    }
    return _nullCollection;
}

}   /* namespace ovengine */ }   /* namespace cinekine */

#endif
