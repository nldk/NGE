#ifndef NGE_LEVEL_H
#define NGE_LEVEL_H
#include <vector>
#include "sprite.h"
#include "json.hpp"
#include <fstream>
class Level {
public:
    std::string name;
    std::vector<Sprite*> sprites;
    Level(std::vector<Sprite*> &sprites,std::string name);
    ~Level() = default;
    void addSprite(Sprite* sprite);
    void render(Scene* scene);
    Error loadLevel();
    void saveLevel();

};

#endif