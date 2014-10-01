///
/// @file
/// Contains the class definition for the SpriteLibrary
///
/// @author     Samir Sinha
/// @date       07/04/13
/// @copyright  Copyright 2013 Cinekine Media
/// @license    The MIT License

#ifndef Overview_SpriteLibrary_hpp
#define Overview_SpriteLibrary_hpp

#include "Engine/Model/Sprite.hpp"

#include "cinek/string.hpp"
#include "cinek/allocator.hpp"
#include "cinek/objectstack.hpp"

#include <unordered_map>

namespace cinekine {
    namespace ovengine {

/**
 * @class SpriteLibrary
 * @brief A store for Sprite objects and their animations
 *
 * The SpriteLibrary acts as a container for Sprite objects.  It also
 * owns Sprite Animation instances.  If a SpriteLibrary instance is destroyed,
 * all of its Sprites are also destroyed.  Any application using
 * SpriteLibrary should take this into account.
 */
class SpriteLibrary
{
    CK_CLASS_NON_COPYABLE(SpriteLibrary);

public:
    /**
     * @param initTemplateLimit The initial SpriteLibrary limit value.  Used
     *                          for efficient usage of memory.  Sprite count can
     *                          exceed the limit, but will result in additional
     *                          memory allocations.
     * @param allocator         An optional allocator.
     */
    SpriteLibrary(size_t initTemplateLimit,
                  const Allocator& allocator=Allocator());

    /**
     * Returns a null sprite, used for initializing empty SpriteInstance objects
     * @return A null sprite
     */
    const Sprite& emptySprite() const {
        return _nullSprite;
    }
    /**
     * Returns a SpriteTemplate ID given its name.
     *
     * @param  templateName SpriteTemplate name.
     * @return If no sprite is found, returns an empty Sprite.
     */
    const Sprite& spriteByName(const std::string& templateName) const;
    /**
     * Returns the animation ID given its state name.
     *
     * @param  animationName Animation state name.
     * @return               Numeric ID for the animation state name.
     */
    AnimationStateId animationIDByName(const char* animationName) const;
    /**
     * Creates a Sprite.
     *
     * @param  name      Name of the template.
     * @param  classId   The bitmap class used for all bitmaps referenced in
     *                   the template.
     * @param  anchor    Anchor pixel offset from origin
     * @param  numStates The animation state count.
     * @return           Pointer to a SpriteTemplate or nullptr if out of
     *                   memory.
     */
    Sprite* createOrModifyFromName(const char* name,
        cinek_bitmap_atlas classId,
        const glm::ivec2& anchor,
        const AABB<Point>& aabb,
        uint16_t numStates);
    /**
     * Maps animation state name to a numeric ID.  Numeric IDs are used
     * to reference animations from SpriteTemplates.  If a state is already
     * mapped to the specified id, the method returns False.
     *
     * @param  name Name of the animation state.
     * @param  id   ID value (can be used to reference a SpriteTemplate
     *              animation.)
     * @return      False if a state already exists with the passed id.
     */
    bool mapAnimationStateNameToId(const char* name,
        AnimationStateId id);

private:
    Allocator _allocator;
    Sprite _nullSprite;
    ObjectStack<Sprite> _templatePool;

    template <typename Key, typename T>
    using unordered_map = std::unordered_map<Key, T,
        std::hash<Key>,
        std::equal_to<Key>,
        std_allocator<std::pair<const Key, T>> >;

    unordered_map<std::string, AnimationStateId> _nameToAnimIds;
    unordered_map<std::string, Sprite*> _nameToTemplates;
};

    }   // namespace ovengine
}   //  namespace cinekine

#endif
