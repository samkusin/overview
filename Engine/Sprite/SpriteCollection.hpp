///
/// @file Sprite/SpriteCollection.hpp
/// Contains the class definition for the SpriteLibrary
///
/// @author     Samir Sinha
/// @date       07/04/13
/// @copyright  Copyright 2013 Cinekine Media
/// @license    The MIT License

#ifndef Overview_SpriteCollection_hpp
#define Overview_SpriteCollection_hpp

#include "Engine/ModelCollection.hpp"
#include "Engine/Sprite/Sprite.hpp"

#include <cinek/string.hpp>
#include <cinek/vector.hpp>

namespace cinek {
    namespace overview {

/**
 * @class SpriteCollection
 * @brief A store for Sprite objects and their animations
 *
 * The SpriteCollection acts as a container for Sprite objects.  It also
 * owns SpriteAnimation instances.  If a SpriteCollection instance is destroyed,
 * all of its Sprites are also destroyed.  Any application using
 * SpriteCollection should take this into account.
 */
class SpriteCollection : public ModelCollection<Sprite>
{
public:
    SpriteCollection() = default;

    /**
     * @param initTemplateLimit The initial SpriteCollection limit value.  Used
     *                          for efficient usage of memory.  Sprite count can
     *                          exceed the limit, but will result in additional
     *                          memory allocations.
     * @param allocator         An optional allocator.
     */
    SpriteCollection(const char* name,
                     vector<Sprite>&& sprites);

    /**
     * Returns a SpriteTemplate ID given its name.
     *
     * @param  templateName SpriteTemplate name.
     * @return If no sprite is found, returns an empty Sprite.
     */
    const Sprite& find(const std::string& templateName) const;
    /**
     * Returns the index of the specified template.
     * @param  templateName SpriteTemplate name
     * @return              Index of the SpriteTemplate or -1 if not found
     */
    int index(const std::string& templateName) const;

private:
    Sprite _nullSprite;
};

    }   // namespace overview
}   //  namespace cinek

#endif
