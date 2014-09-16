///
/// @file
/// Contains the RoomGraph Internal Implementation
///
/// @author     Samir Sinha
/// @date       08/17/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_RoomGraphInterface_Hpp
#define Overview_Model_RoomGraphInterface_Hpp

#include "Engine/Model/RoomGraph.hpp"
#include "cinek/objectheap.hpp"

#include "cinek/vector.hpp"

namespace cinekine {
    namespace ovengine {
        class Portal;
        class RoomGraph;
    }
}

namespace cinekine {
    namespace ovengine {

/// @class   RoomGraphInterface
/// @brief   A non movable, non copyable RoomGraph implementation.
/// @details The persistant implementation of a RoomGraph.
///
class RoomGraph::Impl
{
    CK_CLASS_NON_COPYABLE(Impl);

public:
    Impl(size_t maxRooms, const Allocator& allocator);

    void invalidate();

    Room root() const;

    Room createRoom();

private:
    //  All Room based node objects are meant to be managed only by RoomGraph
    //  or their container class equivalents (Room, Portal, etc)
    //  Also all Node objects are be POD by design, so initialization occurs
    //  when a Room is created.
    void initPortalNode(PortalNode* output, RoomNode* fromRoom, RoomSide side);
    RoomNode* createRoomNode();

    friend class Portal;
    RoomNode* _root;

    ObjectHeap<RoomNode> _roomPool;
    ObjectHeap<PortalNode> _portalPool;
    vector<RoomVertex> _portalVertices;
};

    } /* namespace overview */
} /* namespace cinekine */

#endif