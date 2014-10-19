///
/// @file
/// A Tile Object Library represented by multiple Tile Collections
///
/// @author     Samir Sinha
/// @date       07/04/13
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_TileLibrary_hpp
#define Overview_Model_TileLibrary_hpp

#include "Engine/Model/TileCollection.hpp"
#include "Engine/Model/ModelLibrary.hpp"

#include "cinek/vector.hpp"

namespace cinek { namespace overview {

class TileLibrary: public ModelLibrary<TileCollection, TileSlot, 0xffff>
{
public:
    TileLibrary(SlotType limit, const Allocator& allocator);
    const Tile& tileFromCollectionAtIndex(TileSlot slot, TileIndex index) const;
};

////////////////////////////////////////////////////////////////////////////////
inline TileLibrary::TileLibrary(SlotType limit, const Allocator& allocator) :
    ModelLibrary<TileCollection, TileSlot, 0xffff>(limit, allocator)
{
}

inline const Tile& TileLibrary::tileFromCollectionAtIndex(TileSlot slot, TileIndex index) const
{
    auto& collection = collectionAtSlot(slot);
    return collection[index];
}

}   /* namespace overview */ }   /* namespace cinek */

#endif
