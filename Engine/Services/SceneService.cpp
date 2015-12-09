//
//  SceneService.inl
//  EnginePrototype
//
//  Created by Samir Sinha on 12/02/15.
//
//

#include "SceneService.hpp"
#include "Engine/Scenes/Scene.hpp"
#include "Engine/Debug.hpp"

#include <ckmsg/client.hpp>


namespace cinek {
    namespace ove {

SceneService::SceneService
(
    Scene& context,
    MessageClientSender& sender
) :
    _context(&context),
    _sender(&sender)
{
}

void SceneService::loadScene
(
    const std::string& name,
    std::function<void(const SceneLoadResponse&)> cb
)
{
    SceneLoadRequest req;
    strncpy(req.name, name.c_str(), sizeof(req.name));
    _sender->client->send(_sender->server,
        kMsgSceneLoad,
        makePayloadFromData(req),
        [cb](uint32_t, ckmsg::ClassId cid, const ckmsg::Payload* payload) {
            CK_ASSERT(cid==kMsgSceneLoad);
            cb(*reinterpret_cast<const SceneLoadResponse*>(payload->data()));
        });
}


    } /* namespace ove */
} /* namespace cinek */
