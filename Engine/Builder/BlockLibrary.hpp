///
/// @file
/// Contains the definition for the BlockLibrary class
///
/// @author     Samir Sinha
/// @date       07/14/14
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Buidler_BlockLibrary_hpp
#define Overview_Builder_BlockLibrary_hpp

#include "Engine/Model/ModelLibrary.hpp"
#include "Engine/Builder/BlockCollection.hpp"

namespace cinekine {
    namespace ovengine {

    using BlockLibrary = ModelLibrary<BlockCollection, uint32_t>;

    }   // namespace ovengine
}   //  namespace cinekine

#endif
