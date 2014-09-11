///
/// @file
/// Wrappers for Room related objects
///
/// @author     Samir Sinha
/// @date       08/30/2014
/// @copyright  Copyright 2014 Cinekine Media
/// @license    The MIT License

namespace cinekine {
    namespace ovengine {

    struct RoomNode
    {
        PortalNode* portals[kRoomSide_Count];
        RoomAABB box;
    };

    struct PortalNode
    {
        RoomNode* toRoom;
        RoomNode* fromRoom;
        RoomSide side;
        int iV0;
        int iV1;
    };

    ////////////////////////////////////////////////////////////////////////////

    inline Room::Room(uintptr_t graphptr, RoomNode* infoptr) :
        _graph(graphptr),
        _info(infoptr)
    {
    }

    inline RoomAABB Room::AABB() const
    {
        return _info ? _info->box : RoomAABB();
    }

    ////////////////////////////////////////////////////////////////////////////

    inline Portal::Portal(uintptr_t graphptr, PortalNode* infoptr) :
        _graph(graphptr),
        _info(infoptr)
    {
    }

    inline Portal::operator bool() const
    {
        return _graph!= 0 && _info;
    }

    } /* namespace overview */
} /* namespace cinekine */
