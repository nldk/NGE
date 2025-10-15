#ifndef NGE_SPRITE_H
#define NGE_SPRITE_H
#include <SDL_stdinc.h>
#include "renderer.h"
#include "vec2.h"
#include "error.h"
#include <SDL2/SDL_image.h>

class Texture {
public:
  Uint32* data;
  int width;
  int height;
  Texture();
  Texture(Uint32* data, int width, int height);
  ~Texture(){
    delete[] data;
  };
  Error load(const std::string& path);
};


class Sprite {
public:
  Vec2 pos;
  int width;
  int height;
  Texture* texture;
  Sprite(Texture* texture, float x, float y, int width, int height);
  ~Sprite() = default;
  Error drawSprite(Scene* scene) const;
  bool boxColide(Sprite* sprite);
  bool boxCollide(int x, int y, int w, int h);
  void move(float x, float y);
  Error moveInScene(float x, float y, Scene* scene);
};
#endif