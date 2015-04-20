@0xdde3c996d4fd9e74;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("cinek::ovproto");

struct SystemEvent
{
  const guid :Data = 0x"74 ec 68 0a 3a 7d 41 50 a5 28 f6 d5 1f 72 5f eb";
  type @0 : Int32;
  name @1 : Text;
}
