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
    Room() = default;
    inline Room(uintptr_t graphptr, RoomNode* infoptr);

    operator bool() const { return _graph!=0; }

    Portal portal(RoomSide side) const;

    inline RoomAABB AABB() const;
    inline void setAABB(const RoomAABB& aabb);

    const RoomNode* node() const { return _info; }

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
    Portal() = default;
    inline Portal(uintptr_t graphptr, PortalNode* infoptr);
    operator bool() const { return _graph!= 0; }

    Room toRoom() const;
    Room fromRoom() const;
    Room createRoom(const RoomAABB& aabb=RoomAABB());

    inline RoomVertex startPos() const;
    inline void setStartPos(const RoomVertex& v);
    inline RoomVertex endPos() const;
    inline void setEndPos(const RoomVertex& v);

    const PortalNode* node() const { return _info; }

private:
    uintptr_t _graph;
    PortalNode* _info;
};

    } /* namespace overview */
} /* namespace cinekine */

#include "Engine/Model/Room.inl"

#endif
