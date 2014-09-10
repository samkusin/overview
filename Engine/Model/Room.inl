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
        RoomVertex v0;
        RoomVertex v1;
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
        return _graph!= 0 && _info && _info->v0 != _info->v1;
    }

    inline RoomVertex Portal::startPos() const
    {
        return _info ? _info->v0 : RoomVertex();
    }

    inline void Portal::setStartPos(const RoomVertex& v)
    {
        if (_info)
        {
            _info->v0 = v;
        }
    }

    inline RoomVertex Portal::endPos() const
    {
        return _info ? _info->v1 : RoomVertex();
    }

    inline void Portal::setEndPos(const RoomVertex& v)
    {
        if (_info)
        {
            _info->v1 = v;
        }
    }

    } /* namespace overview */
} /* namespace cinekine */
