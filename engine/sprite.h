#ifndef NGE_SPRITE_H
#define NGE_SPRITE_H
#include <SDL_stdinc.h>
#include "renderer.h"
#include "vec2.h"
class Sprite {
public:
  Vec2 pos;
  int width;
  int height;
  Uint32* texture;
  Sprite(Uint32* texture, float x, float y, int width, int height);
  ~Sprite();
  void drawSprite(Scene* scene);
};

#endif