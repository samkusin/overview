
namespace cinek {


inline bool operator<(const MessageId& l, const MessageId& r)
{
    return (l.classId < r.classId) ||
           (l.classId == r.classId && l.instanceId < r.instanceId);
}

inline bool operator!=(const MessageId& l, const MessageId& r)
{
    return (l.classId != r.classId) || (l.instanceId != r.instanceId);
}


inline Message::Message(MessageId id) :
    _id(id)
{

}

} /* namespace cinek */
