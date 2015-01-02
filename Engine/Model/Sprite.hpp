/**
 * @file    rendermodel/sprite.hpp
 *
 * Sprite related data shared across multiple sprite instances.
 *
 * @note    Created by Samir Sinha on 5/18/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Overview_Model_Sprite_hpp
#define Overview_Model_Sprite_hpp

#include "Engine/Model/SpriteAnimation.hpp"
#include "Engine/Model/AABB.hpp"

#include "cinek/rendertypes.h"
#include "cinek/allocator.hpp"
#include "cinek/objectstack.hpp"
#include "cinek/vector.hpp"
#include "cinek/string.hpp"

#include <glm/glm.hpp>

namespace cinek {
    namespace overview {

/**
 * \class Sprite
 * \brief Defines bitmaps and animation states for a single sprite.
 */
class Sprite
{
    CK_CLASS_NON_COPYABLE(Sprite);

public:
    Sprite();

    /**
     * Constructor reserving memory for a number of states for later definition,
     * serialization.
     *
     * @param name          The sprite name
     * @param bitmapClass   The bitmap class of bitmaps used for this sprite.
     * @param anchor        The sprite's offset from the origin
     * @param numStates     Number of states to reserve in the state table.
     * @param allocator     An optional allocator.
     */
    Sprite(const std::string& name,
           cinek_bitmap_atlas bitmapClass,
           const glm::ivec2& anchor,
           const AABB<Point>& aabb,
           uint16_t numStates,
           const Allocator& allocator=Allocator());
    /** Destructor */
    ~Sprite();
    /** @cond */
    Sprite(Sprite&& other);
    Sprite& operator=(Sprite&& other);
    /** @endcond */

    const std::string& name() const {
        return _name;
    }

    /** @return Returns the bitmap class for bitmaps used in the sprite. */
    cinek_bitmap_atlas getBitmapClass() const {
        return _bitmapClass;
    }
    /** @return The x,y anchor from the sprite's origin */
    const glm::ivec2& anchor() const {
        return _anchor;
    }

    /** @return The Local AABB for the Sprite */
    const AABB<Point>& aabb() const {
        return _aabb;
    }

public:
    /**
     * Create or modify an animation with the specified animation ID.
     *
     * @param  animId     Animation identifier.
     * @param  frameCount The number of frames in the animation.
     * @param  duration   The length of the animation.
     * @return            The animation state created
     */
    SpriteAnimation* createAnimation(AnimationStateId animId,
                                 uint16_t frameCount,
                                 uint32_t duration);

    /**
     * Returns the Animation interface for the specified animation.
     * @param  animId Animation ID.
     * @return        The mapped animation or a placeholder animation if the
     *                animation wasn't initialized.
     */
    SpriteAnimation* getAnimation(AnimationStateId animId) const;

private:
    Allocator _allocator;
    std::string _name;
    cinek_bitmap_atlas _bitmapClass;
    glm::ivec2 _anchor;
    AABB<Point> _aabb;

    //  state table containing animation lists mapped to state.
    ObjectStack<SpriteAnimation> _statePool;
    vector<SpriteAnimation*> _states;
};

    }   // namespace overview
}   //  namespace cinek

#endif
