#include "sprite.h"
#include "vec2.h"
Sprite::Sprite(Uint32 *texture, float x, float y, int width, int height) {
    this->texture = texture;
    this->pos = Vec2(x, y);
    this->width = width;
    this->height = height;
}

void Sprite::drawSprite(Scene* scene) {
    for (int y = 0; y < this->height; y++) {
        int screen_y = this->pos.y + y;
        if (screen_y < 0 || screen_y >= scene->window->height) continue;
        for (int x = 0; x < this->width; x++) {
            int screen_x =this->pos.x + x;
            if (screen_x < 0 || screen_x >= scene->window->width) continue;

            scene->pixels[screen_y * scene->window->width + screen_x] = this->texture[y * this->width + x];
        }
    }
}
Sprite::~Sprite() {
    if (this->texture) {
        delete[] this->texture;
        this->texture = nullptr;
    }
}
