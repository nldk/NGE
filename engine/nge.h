#ifndef NGE_NGE_H
#define NGE_NGE_H

// Central engine header: include SDL and related headers here so other engine files
// can simply #include "nge.h" and get SDL types available.

// Prefer system-installed SDL2 include path
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "button.h"
#include "renderer.h"
#include "color.h"
#include "eventHandeling.h"
#include "level.h"
#include "sprite.h"
#include "sound.h"
#include "vec2.h"

#endif // NGE_NGE_H

