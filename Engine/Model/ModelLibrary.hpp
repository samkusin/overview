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

namespace cinek { namespace overview {

/// @class  ModelLibrary
/// @ingroup Model
/// @brief  A container for ModelCollection records.
///
template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
class ModelLibrary
{
public:
    /// Type ModelCollection type for this library
    typedef _Collection         CollectionType;
    /// The Slot index type for used for collections in this library
    typedef _SlotType           SlotType;
    /// The Invalid Slot index constant
    static const _SlotType      InvalidSlot = _InvalidSlot;

public:
    /// Default Constructor
    ///
    ModelLibrary() = default;
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
    const CollectionType& collectionByName(const std::string& name) const;
    /// Returns the slot that contains the specified collection
    ///
    /// @param  name The collection name
    /// @return The Slot
    ///
    SlotType slotByCollectionName(const std::string& name) const;
    
    /// Retrieves the null collection.
    ///
    /// @return A null/empty ModelCollection.
    ///
    const CollectionType& nullCollection() const;
    /// @return The current maxium number of collection slots.  This is useful
    /// for iteration
    ///
    uint32_t numCollectionSlots() const;
    /// Returns the number of items in the library.  This method will calculate
    /// the total item count iterating through each Collection.
    ///
    /// @return The number of items
    ///
    uint32_t totalCount() const;
    /// Passes a Slot and Collection to the callback for every Collection in
    /// the library.
    ///
    /// @param  cb  The specified functor must have the signature
    ///             bool(SlotType slot, const CollectionType& type)
    /// @return If the callback aborts iteration by returning false, returns
    ///         the Slot of the Collection at that iteration.  Otherwise
    ///         returns the InvalidSlot value.
    template<typename Callback>
    SlotType forEach(Callback cb) const;

protected:
    vector<CollectionType> _collections;
    CollectionType _nullCollection;
};

////////////////////////////////////////////////////////////////////////////////

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
ModelLibrary<_Collection,_SlotType, _InvalidSlot>::ModelLibrary(SlotType collectionLimit,
                                                  const Allocator& allocator) :
    _collections(allocator)
{
    _collections.reserve(collectionLimit);
    for (auto i = 0; i < collectionLimit; ++i)
        _collections.emplace_back(CollectionType());
}

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
void ModelLibrary<_Collection, _SlotType, _InvalidSlot>::clear()
{
    _collections.clear();
}

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
void ModelLibrary<_Collection, _SlotType, _InvalidSlot>::mapCollectionToSlot(
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

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
auto ModelLibrary<_Collection, _SlotType, _InvalidSlot>::collectionAtSlot(SlotType slot) const
    -> const CollectionType&
{
    return _collections[slot % _collections.size()];
}

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
auto ModelLibrary<_Collection, _SlotType, _InvalidSlot>::collectionByName(const std::string& name) const
    -> const CollectionType&
{
    for (auto& collection: _collections)
    {
        if (collection.name() == name)
            return collection;
    }
    return nullCollection();
}

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
auto ModelLibrary<_Collection, _SlotType, _InvalidSlot>::nullCollection() const
    -> const CollectionType&
{
    return _nullCollection;
}

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
auto ModelLibrary<_Collection,
                  _SlotType,
                  _InvalidSlot>::slotByCollectionName(const std::string& name) const
    -> SlotType
{
    for (SlotType slot = 0; slot < _collections.size(); ++slot)
    {
        if (_collections[slot].name() == name)
            return slot;
    }
    return _InvalidSlot;
}

template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
uint32_t ModelLibrary<_Collection,
                  _SlotType,
                  _InvalidSlot>::totalCount() const
{
    uint32_t cnt = 0;
    for (SlotType slot = 0; slot < _collections.size(); ++slot)
    {
        cnt += _collections[slot].count();
    }
    return _InvalidSlot;
}


template<class _Collection, class _SlotType, _SlotType _InvalidSlot>
template<typename Callback>
auto ModelLibrary<_Collection,
                  _SlotType,
                  _InvalidSlot>::forEach(Callback cb) const -> SlotType
{
    for (SlotType slot = 0; slot < _collections.size(); ++slot)
    {
        if (!cb(slot, _collections[slot]))
            return slot;
    }
    return InvalidSlot;
}

}   /* namespace overview */ }   /* namespace cinek */

#endif
