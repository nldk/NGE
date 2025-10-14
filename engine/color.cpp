#include "color.h"

Color::Color(SDL_Color color,SDL_PixelFormat* format) {
    this->color = SDL_MapRGB(format,color.r,color.g,color.b);
}
Color::Color(Uint8 r, Uint8 g, Uint8 b, SDL_PixelFormat* format, Uint8 a) {
    (void)a;
    this->color = SDL_MapRGB(format,r,g,b);
}
Color::Color(Uint32 color) {
    this->color = color;
}
