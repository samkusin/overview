/**
 * @file    Model/Sprite/SpriteAnimation.hpp
 *
 * Sprite animation interface
 *
 * @note    Created by Samir Sinha on 12/30/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Model_SpriteAnimation_hpp
#define Overview_Model_SpriteAnimation_hpp

#include "Engine/ModelTypes.hpp"
#include "CKGfx/GfxTypes.hpp"

namespace cinek {
    namespace overview {

    class SpriteTemplate;

    /**
     * \class State
     * \brief Links animation states to bitmaps.
     */
    class SpriteAnimation
    {
        friend class Sprite;

    public:
        /**
         * Initializes the state.
         * \param id         State identifier.
         * \param frameCount Number of frames in the state animation.
         */
        SpriteAnimation(AnimationStateId id, uint16_t frameCount,
                        gfx::BitmapIndex* frames,
                        uint32_t duration);
        /**
         * Returns the bitmap at the specified frame index.
         * @param  index Frame index.
         * @return       Bitmap index.
         */
        gfx::BitmapIndex getFrame(uint16_t index) const {
            return _frames[index % _frameCount];
        }
        /**
         * Returns the frame at the specified time index.
         * @param  timeValue The time index.
         * @return           The bitmap frame index.
         */
        gfx::BitmapIndex getFrameByTime(uint32_t timeValue) const;
        /**
         * \return Returns the number of frames in the state animation.
         */
        uint16_t getFrameCount() const { return _frameCount; }
        /**
         * \return The State ID.
         */
        AnimationStateId getId() const { return _id; }
        /**
         * Sets the animation frame at the specified index.
         * @param index  Index into animation.
         * @param bitmap Bitmap index.
         */
        void setFrame(uint16_t index, gfx::BitmapIndex bitmap) {
            _frames[index % _frameCount] = bitmap;
        }

    private:
        AnimationStateId _id;
        uint32_t _duration;
        uint16_t _frameCount;
        gfx::BitmapIndex* _frames;
    };

    }   // namespace overview
}   //  namespace cinek

#endif
