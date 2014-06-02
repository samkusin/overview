/**
 * @file    Model/StageSpriteInstance.hpp
 *
 * Contains a Sprite Model instance's state information.
 *
 * \note    Created by Samir Sinha on 5/27/2014.
 *          Copyright (c) 2014 Cinekine. All rights reserved.
 */

#ifndef Overview_StageSpriteInstance_hpp
#define Overview_StageSpriteInstance_hpp

#include "Engine/Model/SpriteInstance.hpp"
#include "Engine/Model/StageTypes.hpp"

namespace cinekine {
    namespace ovengine {

    class Sprite;
    class Stage;

    /**
     * @class StageSpriteInstance
     * @brief A SpriteInstance belonging to a Stage
     */
    class StageSpriteInstance : public SpriteInstance
    {
        CK_CLASS_NON_COPYABLE(StageSpriteInstance);

    public:
        /**
         * Constructs a sprite with the given template.
         * @param spriteTemplate Constructs a sprite based on this template.
         */
        StageSpriteInstance(const Sprite& spriteTemplate, Stage& stage,
                            SpriteInstanceId instanceId);
        /**  Destructor */
        ~StageSpriteInstance();
        /** Move Constructor */
        StageSpriteInstance(StageSpriteInstance&& other);

        operator bool() const {
            return _thisInstanceId != 0;
        }

        SpriteInstanceId spriteInstanceId() const { return _thisInstanceId; }

        /** @return Returns a reference to the owning stage */
        Stage& stage() { return _stage; }
        /** Unlinks the sprite instance from the stage */
        void reset();
        /**
         * Sets the sprite's position on the owning stage
         * @param pos [description]
         */
        void setPosition(const glm::vec3& pos);
        /** @return The map position */
        const glm::vec3& position() {
            return _pos;
        }

    private:
        Stage& _stage;
        glm::vec3 _pos;
        // list node linking to the stage tile
        SpriteInstanceId _thisInstanceId;
        SpriteInstanceId _prevInstanceId;
        SpriteInstanceId _nextInstanceId;

        void detachFromStage();
        void attachToStage();
    };

    } /* rendermodel */
} /* cinekine */

#endif
