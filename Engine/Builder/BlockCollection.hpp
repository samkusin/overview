///
/// @file
/// The BlockCollection class header
///
/// @author     Samir Sinha
/// @date       07/15/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License


#ifndef Overview_Builder_BlockCollection_hpp
#define Overview_Builder_BlockCollection_hpp

#include "Engine/Model/ModelCollection.hpp"
#include "Engine/Builder/Block.hpp"

#include "cinek/string.hpp"

namespace cinekine { namespace ovengine {

/// @class  BlockCollection
/// @ingroup Builder
/// @brief  A group of Block objects sharing attributes
///
class BlockCollection : public ModelCollection<Block>
{
public:
    /// Default constructor
    ///
    BlockCollection() = default;
    /// Constructs a Block collection
    ///
    /// @param name               The collection name
    /// @param blocks             The collection's blocks
    BlockCollection(const char* name,
                    vector<Block>&& blocks);
    /// Returns the tile at the requested index.
    /// @param  name The name of the Block
    /// @return The mapped Block object
    ///
    inline const Type& operator[](const std::string& name) const;


private:
    Block _nullBlock;
};

////////////////////////////////////////////////////////////////////

inline auto BlockCollection::operator[](const std::string& name) const -> const Type&
{
    for (auto& block : _models)
    {
        if (block.name() == name)
            return block;
    }
    return _nullBlock;
}

} /* namespace ovengine */ } /* namespace cinekine */

#endif
