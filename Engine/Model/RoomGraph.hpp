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
#include "cinek/objectheap.hpp"

#include "cinek/vector.hpp"

namespace cinekine {
    namespace ovengine {
        class Portal;
    }
}

namespace cinekine {
    namespace ovengine {

/// @class   RoomGraph
/// @brief   Container owning all objects related to one or more Rooms
/// @details All Room objects and dependencies belong to a RoomGraph.
///
class RoomGraph
{
    CK_CLASS_NON_COPYABLE(RoomGraph);

public:
    RoomGraph(size_t maxRooms, const Allocator& allocator);

    void invalidate();

    Room root() const;

    Room createRoom();

private:
    //  All Room based node objects are meant to be managed only by RoomGraph
    //  or their container class equivalents (Room, Portal, etc)
    //  Also all Node objects are be POD by design, so initialization occurs
    //  when a Room is created.
    void initPortalNode(PortalNode* output, RoomNode* fromRoom, RoomSide side);

    friend class Portal;
    Room _root;

    ObjectHeap<RoomNode> _roomPool;
    ObjectHeap<PortalNode> _portalPool;
    vector<RoomVertex> _portalVertices;
};

    } /* namespace overview */
} /* namespace cinekine */

#endif
