#ifndef NGE_SPRITE_H
#define NGE_SPRITE_H
#include <functional>
#include "renderer.h"
#include "vec2.h"
#include "error.h"
#include "SDL/include/SDL.h"

class Texture {
public:
  std::string path = "";
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
  std::string name;
  std::function<void(Sprite*)> onColide;
  Vec2 pos;
  int width;
  int height;
  Texture* texture;
  Sprite(std::string name,Texture* texture, float x, float y, int width, int height);
  ~Sprite() = default;
  void onClolission(Sprite* other) {
    if (onColide) onColide(other);
  };
  Error drawSprite(Scene* scene) const;
  bool boxCollide(Sprite* sprite);
  bool boxCollide(int x, int y, int w, int h);
  void move(float x, float y);
  Error moveInScene(float x, float y, Scene* scene);
  void setOnCollision(std::function<void(Sprite*)> func) {
    this->onColide = func;
  };

};
#endif