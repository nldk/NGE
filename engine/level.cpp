#include "level.h"

Level::Level(std::vector<Sprite*> &sprites) {
    this->sprites = sprites;
}
void Level::render(Scene *scene) {
    for (auto sprite : sprites) {
        sprite->drawSprite(scene);
    }
}

void Level::addSprite(Sprite *sprite) {
    this->sprites.push_back(sprite);
}



