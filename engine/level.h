#ifndef NGE_LEVEL_H
#define NGE_LEVEL_H
#include <vector>
#include "sprite.h"
class Level {
public:
    std::vector<Sprite*> sprites;
    Level(std::vector<Sprite*> &sprites);
    ~Level() {
        for (auto obj : this->sprites)
            delete obj;
    };
    void addSprite(Sprite* sprite);
    void render(Scene* scene);
    void loadLevel();
};




#endif