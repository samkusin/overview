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
} /* namespace cinekine */

#endif
