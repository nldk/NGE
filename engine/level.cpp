#include "level.h"

#include <iomanip>

Level::Level(std::vector<Sprite *> &sprites,std::string name) {
    this->sprites = sprites;
    this->name = name;
}

void Level::render(Scene *scene) {
    for (auto sprite: sprites) {
        sprite->drawSprite(scene);
    }
}

void Level::addSprite(Sprite *sprite) {
    this->sprites.push_back(sprite);
}

Error Level::loadLevel() {
    std::ifstream file("config.json");
    if (!file.is_open()) {
        return Error(true, "Could not open config.json\n");
    }
    nlohmann::json jsonLevel;
    file >> jsonLevel;
    file.close();
    for (auto& [spritename,data]: jsonLevel["data"].items()) {
        auto* texture = new Texture();
        if (data["textureType"] == "path") {
            Error err = texture->load(data["texture"]);
            if (err.error) {
                return err;
            }
        } else {
            std::string textureData = data["texture"];
            std::istringstream ss(textureData);
            std::vector<Uint32> pixels;
            Uint32 pixel;
            while (ss >> pixel) {
                pixels.push_back(pixel);
            }
        }
        int width = data["width"];
        int height = data["height"];
        Sprite* sprite = new Sprite(
            data["name"],
            texture,
            data["x"],
            data["y"],
            width,
            height
        );
        this->sprites.push_back(sprite);
    }
    return Error(false, "No error");
}
std::string concatTextures(Uint32* data,int length, int width) {
    std::string dataString = "";
    for (int i = 0; i < length * width; i++) {
        dataString += std::to_string(data[i]) + " ";
    }
    return dataString;
}
void Level::saveLevel() {
    nlohmann::json jsonLevel;
    for (Sprite *sprite: sprites) {
        if (sprite->texture->path == "") {
            jsonLevel["sprites"][sprite->name] = {
                {"textureType", "data"},
                {"texture", concatTextures(sprite->texture->data, sprite->texture->height, sprite->texture->width)},
                {"name", sprite->name},
                {"x", sprite->pos.x},
                {"y", sprite->pos.y},
                {"width", sprite->width},
                {"height", sprite->height}
            };
        } else {
            jsonLevel["sprites"][sprite->name] = {
                {"textureType","path"},
                {"texture", sprite->texture->path},
                {"name", sprite->name},
                {"x", sprite->pos.x},
                {"y", sprite->pos.y},
                {"width", sprite->width},
                {"height", sprite->height}
            };
        }
    }
    std::ofstream file(this->name);
    file << std::setw(4) << jsonLevel;
    file.close();
}
