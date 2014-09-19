///
/// @file
/// Wrappers for Room related objects
///
/// @author     Samir Sinha
/// @date       08/29/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

#ifndef Overview_Model_Room_Hpp
#define Overview_Model_Room_Hpp

#include "Engine/Model/RoomTypes.hpp"
#include "Engine/Model/AABB.hpp"

namespace cinekine {
    namespace ovengine {

struct RoomNode;
struct PortalNode;

class RoomGraph;
class Portal;

/// @class   Room
/// @brief   A Builder object surrounded by four edges that connect to other
///          Room objects
/// @details The Room is the primary node within a Room graph.  Connectivity
///          between Rooms is specified using Edge and Portal objects.
///          Every Room and its components belong to a RoomGraph.  Components
///          are referenced by index to arrays within the RoomGraph.
///
class Room
{
public:
    typedef RoomAABB                    AABB;
    typedef AABB::point_type            point_type;

    Room() = default;
    inline Room(uintptr_t graphptr, RoomNode* infoptr);

    operator bool() const { return _graph!=0; }

    //  clears all portals, and resets edges to the AABB
    void resetToBounds(const RoomAABB& aabb);

    Portal portal(RoomSide side) const;

    inline RoomAABB aabb() const;

    const RoomNode* node() const { return _info; }

    inline bool operator==(const Room& rhs) const;
    inline bool operator!=(const Room& rhs) const;

private:
    friend class Portal;

    uintptr_t _graph;
    RoomNode* _info;
};

/// @class   Portal
/// @brief   Links two rooms together via Edge objects.
/// @details Portals link two edges together.  Since every Edge belongs to a
///          Room, a Portal implicitly links two Rooms together.  The vertices
///          define a 'hole' between the two edges.
///
class Portal
{
public:
    typedef RoomVertex::value_type offset_type;

    Portal() = default;
    inline Portal(uintptr_t graphptr, PortalNode* infoptr);
    inline operator bool() const;

    Room toRoom() const;
    Room fromRoom() const;

    ///  Creates a room attached to this portal, connecting it to the portal's
    ///  owning room.   Note the portal for the newly created room's connecting
    ///  side is automatically set to match this portal's starting and end pos.
    ///
    ///  @param  aabb  The room's bounding box
    ///  @return The created room interface
    Room createRoom(const RoomAABB& aabb=RoomAABB());

    /// Set the portal points using an offset from the room side's start
    /// position.
    ///
    /// @param  offset The offset from the parent room's side start point.
    ///                The value offsets either from the x or y start coordinate
    ///                depending on the portal's side.
    /// @param  length The length of the portal.
    void setFromOffsets(offset_type offset, offset_type length);

    /// Set portal positions to the edges
    void setAsOpen();

    /// Set portal positions to the end edge, closing the wall
    void setAsClosed();

    RoomVertex startPos() const;
    void setStartPos(const RoomVertex& v);
    RoomVertex endPos() const;
    void setEndPos(const RoomVertex& v);

    RoomVertex edgeStartPos() const;
    RoomVertex edgeEndPos() const;

    const PortalNode* node() const { return _info; }

    inline bool operator==(const Portal& rhs) const;
    inline bool operator!=(const Portal& rhs) const;

private:
    uintptr_t _graph;
    PortalNode* _info;
};

    } /* namespace overview */
} /* namespace cinekine */

#include "Engine/Model/Room.inl"

#endif
