/**
 *  @file   IMGUITypes.hpp
 *
 *  @note   Created by Samir Sinha on 9/12/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef Cinekine_IMGUITypes_hpp
#define Cinekine_IMGUITypes_hpp

#include <cstdint>
#include <cstddef>
#include <limits>

#if CK_CPP_EXCEPTIONS
#define CK_IMGUI_EXCEPTIONS 1
#endif

namespace cinekine {
    namespace imgui {

/**
 * @struct Rect
 * @brief  Yet another rectangle data object.
 */
struct Rect
{
    int32_t x;              /**< The left coordinate */
    int32_t y;              /**< The top coordinate */
    int32_t w;              /**< The width */
    int32_t h;              /**< The height */
};

/**
 * @enum  Alignment
 * @brief Specifies the alignment of a UI element.
 */
enum Alignment
{
    kAlignLeft,             /**< Aligns to the left */
    kAlignCenter,           /**< Center alignment */
    kAlightRight            /**< Right alignment */
};

/**
 * @struct ColorRGBA
 * @brief  Red, green, blue, alpha components for a color.
 */
struct ColorRGBA
{
    uint8_t r;              /**< Red component */
    uint8_t g;              /**< Green component */
    uint8_t b;              /**< Blue component */
    uint8_t a;              /**< Alpha component */
};

/** Black color */
const ColorRGBA kBlack = { 0,0,0,255 };
/** White color */
const ColorRGBA kWhite = { 255,255,255,255 };

/**
 * @struct Input
 * @brief  Consolidated input into a Frame.
 */
struct Input
{
    enum 
    {
        kMBtnLeft   = 0x01,
        kMBtnRight  = 0x02
    };

    int32_t mX;             /**< The Mouse X position. */
    int32_t mY;             /**< The Mouse Y position. */
    int32_t mScroll;        /**< The Mouse Scroll position. */
    uint8_t mBtnMask;       /**< The Mouse Button state. */
};

/**
 * @struct DrawCommand
 * @brief  Applications can use DrawCommand lists supplied by a Frame to
 *         render GUIs in a render frame.
 */
struct DrawCommand
{
    /** Draw command type */
    enum Type
    {
        kLine,              /**< Draws a line. */
        kText,              /**< Prints a line of text. */
        kTriangle,          /**< Draws a triangle. */
        kRect,              /**< Draws a rectangle. */
        kScissor            /**< Scissor. */
    };
    Type type;              /**< Type of command. */
    ColorRGBA color;        /**< Color of primitive. */
    union
    {
        struct {
            Rect rect;      /**< Rectangle. */
            int32_t r;      /**< Rounded rectangle radius. */
        } rectParam;        /**< Used for rectangles and triangles. */
        struct {
            int32_t x0;     /**< X start coord. */
            int32_t y0;     /**< Y start coord. */
            int32_t x1;     /**< X end coord. */
            int32_t y1;     /**< Y end coord. */
        } lineParam;        /**< Used for lines. */
        struct {
            const char* text;   /**< Text C string. */ 
            Alignment align;    /**< Text alignment relative to anchor. */
            int32_t x;          /**< X anchor coord. */
            int32_t y;          /**< Y end coord. */
        } textParam;        /**< Used for text labels. */
    };
};

struct Allocator
{
    Allocator();

    void* cxt;
    void* (*alloc)(void* cxt, size_t sz);
    void (*free)(void* cxt, void *p);
};

/**
 * @class imgui_allocator
 * @brief A std::allocator compliant allocator for STL containers.
 */
template <typename T>
struct imgui_allocator
{
    /** @cond */
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    
    template <class U> struct rebind { typedef imgui_allocator<U> other; };
    imgui_allocator() {}
    imgui_allocator(const Allocator& allocator): _allocator(allocator) {}
    imgui_allocator(const imgui_allocator& source): _allocator(source._allocator) {}
    template <class U> imgui_allocator(const imgui_allocator<U>& source): _allocator(source._allocator) {}
    ~imgui_allocator() {}
    
    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    
    pointer allocate(size_type s, const void* = 0)
    {
        if (s == 0)
            return nullptr;
        pointer temp = static_cast<pointer>(_allocator.alloc(_allocator.cxt, s*sizeof(T)));
        
#if CK_IMGUI_EXCEPTIONS
        if (temp == nullptr)
            throw std::bad_alloc();
#endif
        return temp;
    }
    void deallocate(pointer p, size_type)
    {
        _allocator.free(_allocator.cxt, (void* )p);
    }
    size_type max_size() const
    {
        return std::numeric_limits<size_t>::max() / sizeof(T);
    }
    template<class U, class... Args> void construct(U* p, Args&&... args)
    {
        ::new((void *)p) U(std::forward<Args>(args)...);
    }
    void destroy(pointer p)
    {
        p->~T();
    }
    
    Allocator _allocator;
    /** @endcond */
};

/** @cond */
template<typename T>
inline bool operator==(const imgui_allocator<T>& lha, const imgui_allocator<T>& rha) {
    return lha._allocator == rha._allocator;
}
template<typename T>
inline bool operator!=(const imgui_allocator<T>& lha, const imgui_allocator<T>& rha) {
    return lha._allocator != rha._allocator;
}
/** @endcond */

    }   // namespace imgui
}   // namespace cinekine

#endif

