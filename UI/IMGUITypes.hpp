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
#include <vector>
#include <array>

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
 * @struct Box
 * @brief  Used to emulate a box model for layout engines.
 */
struct Box
{
    int32_t l;              /**< Left value */
    int32_t t;              /**< Top value */
    int32_t r;              /**< Right value */
    int32_t b;              /**< Bottom value */
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

/**
 * @enum DrawStyle
 * @brief Defines how the primative is drawn.
 */
enum DrawStyle
{
    kDrawSolid              /**< A solid line/fill. */
};

/** Transparent color. */
const ColorRGBA kTransparent = { 0,0,0,0 };
/** Black color */
const ColorRGBA kBlack = { 0,0,0,255 };
/** White color */
const ColorRGBA kWhite = { 255,255,255,255 };

/** Defines Fonts by index/handle. */    
typedef uint32_t FontHandle;
/** The 'default' found */
const FontHandle kFontHandle_Default = (FontHandle)(0);

/**
 * @struct Style
 * @brief Defines styling information for widgets.
 *
 * Attempts to emulate CSS styling
 *
 * Note, all units are in IMGUI units (virtual pixels). 
 */
struct Style
{
    struct 
    {
        ColorRGBA color;    /**< Background color. */
    }
    background;             /**< Background styling. */
    struct
    {
        std::array<DrawStyle, 4> styles;    /**< Line style for the border. */
        std::array<ColorRGBA, 4> colors;    /**< Border line color. */
        std::array<int16_t, 4> widths;      /**< Defines border widths for a rect. */         
        int32_t radius;     /**< Used for rounded corners. */
    }
    border;
    Box padding;            /**< Padding rectangle */
    Box margin;             /**< Margin rectangle */

    ColorRGBA color;        /**< Color for all foreground elements. */
    FontHandle font;

    Style()
    {
        color = kWhite;
        background.color = kTransparent;
        border.widths = { 0, 0, 0, 0 };
        padding = { 0, 0, 0, 0 };
        margin = { 0, 0, 0, 0 };
        font = kFontHandle_Default;
    } 

    /** @return True if there is a border element. */
    bool hasBorder() const {
        return border.widths[0] || border.widths[1] || border.widths[2] || border.widths[3];
    }
};

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
 * @struct LineCommand
 * @brief  Defines a line.
 */
struct LineCommand
{
    int32_t x0;         /**< X start coord. */
    int32_t y0;         /**< Y start coord. */
    int32_t x1;         /**< X end coord. */
    int32_t y1;         /**< Y end coord. */
    int32_t thickness;  /**< Thickness. */
};

/**
 * @struct RectCommand
 * @brief  Defines a rectangle.
 */
struct RectCommand
{
    Rect r;                  /**< Rectangle descriptor. */
    int32_t cornerRadius;    /**< Rounded corner radius. */
};

/** 
 * @struct TextCommand
 * @brief  Defines a text label.
 */
struct TextCommand
{
    const char* str;
    Alignment align;
    int32_t x;
    int32_t y;
};

/**
 * @struct DrawCommand
 * @brief  Applications can use DrawCommand lists supplied by a Frame to
 *         render GUIs in a render frame.
 */
struct DrawCommand
{
    /** Max number of elements per draw command */
    static const size_t kMaxElements = 4;
    /** Draw command type */
    enum Type
    {
        kLine,              /**< Draws a line. */
        kText,              /**< Prints a line of text. */
        kTriangle,          /**< Draws a triangle. */
        kRect,              /**< Draws a rectangle. */
        kFillRect,          /**< Fills a rectangle. */
        kScissor            /**< Scissor. */
    };
    Type type;              /**< Type of command. */
    std::array<DrawStyle, kMaxElements> styles;  /**< Draw styles */
    std::array<ColorRGBA, kMaxElements> colors;  /**< Color of primitive. */
    std::array<int16_t, kMaxElements> widths;    /**< Element widths */
    DrawCommand(Type _type) : type(_type) {}
    DrawCommand(Type _type, DrawStyle _style, ColorRGBA _color) :
        styles( {_style} ), colors( {_color} ) {}

    union
    {
        LineCommand line;   /**< Used for line commands */
        RectCommand rect;   /**< Used for rectangle commands */
        TextCommand text;   /**< Used for text commands */
    };
};

/**
 * @struct Allocator
 * @brief A custom memory allocator supplied by applications to Gfx classes.
 */
struct Allocator
{
    Allocator();

    void* cxt;
    void* (*alloc)(void* cxt, size_t sz);
    void (*free)(void* cxt, void *p);
};

/**
 * @class StringStack
 * @brief Manages a stack of C strings.
 */
class StringStack
{
public:
    StringStack(Allocator& allocator, size_t size) :
        _allocator(allocator),
        _base((char*)(*_allocator.alloc)(_allocator.cxt, size)),
        _top(_base),
        _limit(_base+size)
        {}
    ~StringStack() {
        (*_allocator.free)(_allocator.cxt, _base);
    }
    /**
     * Pushes a string onto the stack
     * @param  str A C string
     * @return     Pointer to the copied string as it resides on the
     *             stack
     */
    const char* push(const char* str) {
        if (_top == _limit)
            return NULL;
        const char* start = _top;
        //  TODO: utf8 clipping
        while (_top < _limit-1 && *str) {
            *_top = *str;
            ++_top;
            ++str;
        }
        *_top = '\0';
        ++_top;
        return start;
    }
    /** Resets the string stack. */
    void reset() {
        _top = _base;
    }
    /** 
     * @return Returns the number of characters remaining free in the
     *         stack
     */
    size_t remaining() const {
        return (_limit - _top);
    }
private:
    Allocator& _allocator;
    char* _base;
    char* _top;
    char* _limit;
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

/** A DrawCommand vector typically owned by the IMGUIFrame */
typedef std::vector<DrawCommand, imgui_allocator<DrawCommand>> DrawCommands;

    }   // namespace imgui
}   // namespace cinekine

#endif

