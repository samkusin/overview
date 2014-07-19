///
/// @file
/// The BlockCollection class header
///
/// @author     Samir Sinha
/// @date       07/15/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Model_BlockCollection_hpp
#define Overview_Model_BlockCollection_hpp

#include "Engine/Model/ModelCollection.hpp"
#include "Engine/Model/Block.hpp"

#include "cinek/string.hpp"

namespace cinekine { namespace ovengine {

/// @class  BlockCollection
/// @ingroup TileModel
/// @brief  A group of Block objects sharing attributes
///
class BlockCollection : public ModelCollection<Block>
{
public:
    BlockCollection(const char* name, const char* tileCollectionName,
                    vector<Block>&& blocks);
    /// Returns the tile at the requested index.
    /// @param  name The name of the Block
    /// @return The mapped Block object
    ///
    const Type& operator[](const string& name) const;

    /// @return The TileCollection name
    ///
    const string& tileCollectionName() const;

private:
    string _tileCollectionName;
    Block _nullBlock;
};

////////////////////////////////////////////////////////////////////

inline auto BlockCollection::operator[](const string& name) const -> const Type&
{
    for (auto& block : _models)
    {
        if (block.name() == name)
            return block;
    }
    return _nullBlock;
}

inline const string& BlockCollection::tileCollectionName() const
{
    return _tileCollectionName;
}

} /* namespace ovengine */ } /* namespace cinekine */

#endif
