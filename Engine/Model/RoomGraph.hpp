///
/// @file
/// Contains the RoomGraph
///
/// @author     Samir Sinha
/// @date       08/17/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_RoomGraph_Hpp
#define Overview_Model_RoomGraph_Hpp

#include "Engine/Model/Room.hpp"
#include "cinek/allocator.hpp"

namespace cinek {
    namespace overview {
        class Portal;
    }
}

namespace cinek {
    namespace overview {

/// @class   RoomGraph
/// @brief   Container owning all objects related to one or more Rooms
/// @details All Room objects and dependencies belong to a RoomGraph.  To
///          maintain relationships between Room and Portal nodes with their
///          owning RoomGraph, RoomGraph is *guaranteed* not to alter or remap
///          nodes to memory once allocated -- basically all pointers remain
///          valid for the life of the RoomGraph.
///          Remember this when invoking move operators on a RoomGraph.  Any
///          Node objects remaining after a move operation on RoomGraph will
///          belong to the moved-to RoomGraph.
///          Especially remember this when destroying a RoomGraph!
///
class RoomGraph
{
    CK_CLASS_NON_COPYABLE(RoomGraph);

public:
    RoomGraph(size_t maxRooms, const Allocator& allocator);
    RoomGraph(RoomGraph&& other);
    ~RoomGraph();

    RoomGraph& operator=(RoomGraph&& other);

    void invalidate();

    Room root() const;

    Room createRoom();

private:
    friend class Portal;
    class Impl;
    unique_ptr<Impl> _impl;
};

    } /* namespace overview */
} /* namespace cinek */

#endif
