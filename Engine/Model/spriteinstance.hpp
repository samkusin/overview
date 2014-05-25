/**
 * @file    Model/SpriteInstance.hpp
 *
 * Contains a Sprite Model instance's state information.
 *
 * @note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_SpriteInstance_hpp
#define Overview_SpriteInstance_hpp

#include "Engine/Model/ModelInstance.hpp"
#include "cinek/types.hpp"

#include <glm/glm.hpp>

namespace cinekine {
    namespace ovengine {

    class Sprite;
    class SpriteAnimation;

    /**
     * @class SpriteInstance
     * @brief A model representing a collection of one of more bitmaps.
     */
    class SpriteInstance: public ModelInstance
    {
        CK_CLASS_NON_COPYABLE(SpriteInstance);

    public:
        /**
         * Constructs a sprite with the given template.
         * @param spriteTemplate Constructs a sprite based on this template.
         */
        SpriteInstance(const Sprite& spriteTemplate);
        /**
         * Destructor
         */
        virtual ~SpriteInstance();

        /**
         * Returns the bitmap atlas (ID) associated with this sprite.  All
         * animation frames related to a sprite must reside within the same atlas
         * which is why a sprite has one and only one atlas.
         *
         * @return The bitmap atlas ID
         */
        cinek_bitmap_atlas getBitmapAtlas() const;
        /**
         * @return Returns the number of frames in the state animation.
         */
        uint16_t getBitmapFrameCount() const;
        /**
         * Returns a bitmap index given a time value following the start time
         * specified when setting the state.  As long as the start time value and
         * this time value are related, this method returns what bitmap to render
         * at the current time.
         *
         * @param  currentTime Time value following the time specified in setState
         * @return             [description]
         */
        cinek_bitmap_index getBitmapFromTime(uint32_t currentTime) const;
         /**
         * Returns the bitmap at the specified frame index.
         * @param  index Frame index.
         * @return       Bitmap index.
         */
        cinek_bitmap_index getBitmapFrame(uint16_t index) const;

        /**
         * Sets the sprite's state.  State relies on the sprite template's
         * state table of bitmaps.
         * @param stateId   The animation state
         * @param startTime The animation start time used to time animations
         */
        virtual void setState(AnimationStateId stateId,
                              uint32_t startTime);

    private:
        const Sprite& _template;
        uint32_t _startTime;
        AnimationStateId _stateId;
        SpriteAnimation* _animation;
        glm::vec3 _worldPos;
    };

    } /* rendermodel */
} /* cinekine */

#endif