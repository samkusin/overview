@0xaba09a976dfaf029;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("cinek::overview::msg");

using EntityId = UInt64;

struct Vector3
{
    x @0 : Float64;
    y @1 : Float64;
    z @2 : Float64;
}

struct Color
{
    a @0 : UInt8;
    b @1 : UInt8;
    g @2 : UInt8;
    r @3 : UInt8;
}

struct Transform
{
    position  @0 : Vector3;
    frontDir  @1 : Vector3;
    upDir     @2 : Vector3;
    scale     @3 : Vector3;
}
