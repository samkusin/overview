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

#include "Engine/Model/ModelTypes.hpp"
#include "Engine/Model/AABB.hpp"
#include "cinek/intrusive_list.hpp"

#include <glm/glm.hpp>

namespace cinekine {
    namespace ovengine {

    class Sprite;
    class SpriteAnimation;
    class SpriteInstance;

    using SpriteInstanceList = intrusive_list<SpriteInstance>;

    /**
     * @class SpriteInstance
     * @brief A Sprite model instance representing a collection of one of more
     *        bitmaps.
     */
    class SpriteInstance
    {
        CK_CLASS_NON_COPYABLE(SpriteInstance);

        static const Sprite kNullSprite;

    public:
        SpriteInstance();
        /**
         * Constructs a sprite with the given template.
         * @param spriteTemplate Constructs a sprite based on this template.
         */
        SpriteInstance(const Sprite& spriteTemplate);
        /**
         * Destructor
         */
        ~SpriteInstance();

        SpriteInstance(SpriteInstance&& other);

        /**
         * @return Returns the number of frames in the state animation.
         */
        uint16_t bitmapFrameCount() const;
        /**
         * Returns a bitmap  given a time value following the start time
         * specified when setting the state.  As long as the start time value and
         * this time value are related, this method returns what bitmap to render
         * at the current time.
         *
         * @param  currentTime Time value following the time specified in setState
         * @return             [description]
         */
        cinek_bitmap bitmapFromTime(uint32_t currentTime) const;
         /**
         * Returns the bitmap at the specified frame index.
         * @param  index Frame index.
         * @return       Bitmap index.
         */
        cinek_bitmap bitmapFrame(uint16_t index) const;

        /**
         * Sets the sprite's state.  State relies on the sprite template's
         * state table of bitmaps.
         * @param stateId   The animation state
         * @param startTime The animation start time used to time animations
         */
        void setState(AnimationStateId stateId,
                      uint32_t startTime);

        /** @return The world position of the sprite */
        const Point& position() const {
            return _position;
        }
        /** @return A r/w reference to the sprite's position */
        void setPosition(const Point& pos) {
            _position = pos;
        }
        /** @return The local AABB of this sprite instance */
        const AABB<Point>& aabb() const;
        /** @return The 2D anchor pixel offset from center, bottom */
        const glm::ivec2& anchor() const;

    private:
        friend SpriteInstanceList;

        SpriteInstance* __prevListNode;
        SpriteInstance* __nextListNode;

    private:
        const Sprite& _template;
        Point _position;
        uint32_t _startTime;
        AnimationStateId _stateId;
        //  cached pointer from the Sprite template
        const SpriteAnimation* _animation;
    };

    } /* rendermodel */
} /* cinekine */

#endif
