#ifndef NGE_COLOR_H
#define NGE_COLOR_H
#include <SDL_pixels.h>
#include <SDL_stdinc.h>

class Color {
    public:
     Uint32 color;
    Color(SDL_Color color,SDL_PixelFormat* format);
    Color(Uint8 r, Uint8 g, Uint8 b, SDL_PixelFormat* format, Uint8 a = 255);
    Color(Uint32 color);
    ~Color() = default;
    Uint32 get() const { return color; }
};

#endif