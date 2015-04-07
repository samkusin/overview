
#include <cinek/allocator.hpp>

#define STBI_MALLOC(_sz_) cinek::cinek_alloc(0, (_sz_))
#define STBI_REALLOC(_ptr_, _sz_) cinek::cinek_realloc(0, (_ptr_), (_sz_))
#define STBI_FREE(_ptr_) cinek::cinek_free(0, (_ptr_))

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
