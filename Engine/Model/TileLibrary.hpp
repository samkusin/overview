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

namespace cinekine { namespace ovengine {

class TileLibrary: public ModelLibrary<TileCollection, TileSlot>
{
public:
    const Tile& tileFromCollectionAtIndex(TileSlot slot, TileIndex index) const;
};

////////////////////////////////////////////////////////////////////////////////

const Tile& TileLibrary::tileFromCollectionAtIndex(TileSlot slot, TileIndex index) const
{
    auto& collection = collectionAtSlot(slot);
    return collection[index];
}

}   /* namespace ovengine */ }   /* namespace cinekine */

#endif
