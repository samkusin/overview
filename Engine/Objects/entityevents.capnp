@0xa487d621048a219a;

using Cxx = import "/capnp/c++.capnp";
using Overview = import "ovtypes.capnp";

$Cxx.namespace("cinek::overview::msg");

# Request a new entity from a template.
#
struct CreateEntityRequest
{
    templateName    @0 : Text;
}

# Notfication of a newly created entity.
#
struct EntityCreatedEvent
{
    entityId    @0 : Overview.EntityId;
}

# Notification when an entity has moved.
#
struct EntityMovedEvent
{
    entityId    @0 : Overview.EntityId;
    transform   @1 : Overview.Transform;
}
