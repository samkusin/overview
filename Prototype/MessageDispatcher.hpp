///
/// @file MessageDispatcher.hpp
///
/// @author Samir Sinha
/// @date 11/15/14.
/// @copyright CineKine. All rights reserved.
///

#ifndef Overview_Game_MessageDispatcher_hpp
#define Overview_Game_MessageDispatcher_hpp

#include "MessageStream.hpp"

#include <cinek/objectpool.hpp>
#include <cinek/vector.hpp>
#include <cinek/map.hpp>
#include <capnp/message.h>
#include <functional>

namespace cinek { namespace overview {

class MessageQueue;

/// @class MessageDispatcher
/// @brief Implements queuing and dispatch of messages from a MessageStream
///        using the Cap'n Proto messaging system
///
class MessageDispatcher
{
public:
    ~MessageDispatcher();

    /// Constructs a dispatcher, preallocating buffers used for delegating
    /// messages.
    ///
    /// @param  delegateCount Estimated number of concurrently registered
    ///                       delegates
    /// @param  classCount    Estimated number of class specific delegates
    /// @param  sequenceCount Estimated number of sequenced/command delegates
    ///
    MessageDispatcher
    (
        size_t delegateCount=32,
        size_t classCount=16,
        size_t sequenceCount=16,
        Allocator allocator=Allocator()
    );
    /// The message delegate
    using Delegate =
        std::function<void(capnp::MessageReader&,
                           MessageHeader::SequenceId,
                           void*)>;
    ///
    /// Queues a message for dispatch
    ///
    /// @param  stream   The target where to stream the message
    /// @param  msg      The message object to stream
    /// @param  msgClass The Message Class
    /// @param  cb       (Optional) Callback issued upon dispatch
    /// @return A handle used for cancelling delegate dispatch
    ///
    MessageDelegateHandle queue
    (
        MessageStream& stream,
        capnp::MessageBuilder& msg,
        const MessageHeader::ClassId& msgClass,
        const Delegate& cb=Delegate()
    );
    ///
    /// Subscribe to a specific message type
    /// @param  msgClass The Message Class
    /// @param  del      The delegate executed when a message of class 'id' is
    ///                  processed
    /// @return A handle to the subscribed delegate.  Subscribers must
    ///         unsubscribe at some point if the subscriber id destroyed before
    ///         the MessageDispatcher
    ///
    MessageDelegateHandle on
    (
        const MessageHeader::ClassId& msgClass,
        const Delegate& del
    );
    ///
    /// Unsubscribes a delegate.  This must occur before the delegate owner is
    /// destroyed.
    ///
    /// @param delegateHandle The delegate object handle.
    ///
    void remove(MessageDelegateHandle delegateHandle);
    ///
    /// Process all events in the queue.  At the end of this method, the queue
    /// is flushed.
    ///
    /// @param queue   The message queue to process
    /// @param context Application supplied context
    ///
    void dispatch
    (
        MessageStream& stream,
        void* context=nullptr
    );
private:
    MessageHeader::SequenceId _nextSeqId;
    MessageDelegateHandle _nextDelegateHandle;

    // Store for Delegates by their delegate handle
    struct DelegateEntry
    {
        MessageDispatcher::Delegate del;
        DelegateEntry* next = nullptr;
        DelegateEntry* prev = nullptr;
        MessageDelegateHandle handle = 0;          // ref to its owner
        MessageHeader::SequenceId seqId = 0;
        int32_t refcnt = 0;
    };
    ObjectPool<DelegateEntry> _delegatePool;
    unordered_map<MessageDelegateHandle, DelegateEntry*> _delegates;

    void obtainDelegateEntry(DelegateEntry* entry);
    void releaseDelegateEntry(DelegateEntry* entry);

    // These maps contain references to DelegateEntry objects from the
    // _delegates and _delegatePool containers.
    // The _classDelegates vector contains a head node of a delegate list
    // where each sibling node resides within the _delegates array
    vector<std::pair<MessageHeader::ClassId, DelegateEntry*>> _classDelegates;
    unordered_map<MessageHeader::SequenceId, DelegateEntry*> _sequenceDelegates;

    template<typename Key, typename Container>
    auto lowerBoundIterator(const Key& key, Container& container) ->
        typename Container::iterator
    {
        return std::lower_bound(container.begin(), container.end(),
                                key,
                               [](const typename Container::value_type &l,
                                  const Key& r) -> bool
                               {
                                   return l.first < r;
                               });
    }
};

} /* namespace overview */ } /* namespace cinek */

#endif
