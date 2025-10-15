#include "sprite.h"
#include "vec2.h"
#include <SDL2/SDL_image.h>
Sprite::Sprite(Texture* texture, float x, float y, int width, int height) {
    this->texture = texture;
    this->pos = Vec2(x, y);
    this->width = width;
    this->height = height;
}

Error Sprite::drawSprite(Scene* scene) const {
    if (this->texture == nullptr || this->texture->data == nullptr) {
        return Error(true,"Texture is null");
    }
    if (scene == nullptr || scene->pixels == nullptr) {
        return Error(true,"Scene is null");
    }
    if (scene->window == nullptr) {
        return Error(true,"Scene window is null");
    }
    if (this->width <= 0 || this->height <= 0 || this->texture->width <= 0 || this->texture->height <= 0) {
        return Error(true, "Invalid sprite or texture dimensions");
    }

    // For each destination pixel in the sprite, sample the corresponding source pixel
    // from the texture using nearest-neighbor scaling.
    for (int dy = 0; dy < this->height; dy++) {
        int screen_y = static_cast<int>(this->pos.y) + dy;
        if (screen_y < 0 || screen_y >= scene->window->height) continue;

        // Compute source y once per row
        int src_y = static_cast<int>((static_cast<long long>(dy) * this->texture->height) / this->height);
        // Clamp just in case of rounding edge
        if (src_y >= this->texture->height) src_y = this->texture->height - 1;

        for (int dx = 0; dx < this->width; dx++) {
            int screen_x = static_cast<int>(this->pos.x) + dx;
            if (screen_x < 0 || screen_x >= scene->window->width) continue;

            int src_x = static_cast<int>((static_cast<long long>(dx) * this->texture->width) / this->width);
            if (src_x >= this->texture->width) src_x = this->texture->width - 1;

            // Correct indexing: use texture->width for source stride
            Uint32 src_pixel = this->texture->data[src_y * this->texture->width + src_x];
            scene->pixels[screen_y * scene->window->width + screen_x] = src_pixel;
        }
    }
    return Error(false,"no error");
}

Texture::Texture() {
    this->data = nullptr;
    this->width = 0;
    this->height = 0;
}
Texture::Texture(Uint32* data, int width, int height) {
    this->width = width;
    this->height = height;
    this->data = new Uint32[width * height];
    memcpy(this->data, data, width * height * sizeof(Uint32));
}
Error Texture::load(const std::string& path) {
    auto init = IMG_Init(0);
    if (!(init & IMG_INIT_PNG)) {
        IMG_Init(IMG_INIT_PNG);
    }
    if (!(init & IMG_INIT_JPG)) {
        IMG_Init(IMG_INIT_JPG);
    }
    SDL_Surface* texture = IMG_Load(path.c_str());
    if (texture == nullptr) {
        return Error(true, "Could not load image becous e: " + std::string(IMG_GetError()));
    }
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(texture, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(texture);
    if (converted == nullptr) {
        return Error(true, "Could not convert image becous e: " + std::string(SDL_GetError()));
    }
    Uint32* pixels = (Uint32*) converted->pixels;
    this->data = new Uint32[converted->w * converted->h];
    memcpy(this->data, pixels, converted->w * converted->h * sizeof(Uint32));
    this->width = converted->w;
    this->height = converted->h;
    SDL_FreeSurface(converted);
    return Error(false, "No Error");
}

bool Sprite::boxColide(Sprite *sprite) {
    float disx = fabs(this->pos.x - sprite->pos.x);
    float disy = fabs(this->pos.y - sprite->pos.y);
    return (disx < sprite->width +this->width && disy < sprite->height+this->height);
}

bool Sprite::boxCollide(int x, int y, int w, int h) {
    float disx = fabs(x - this->pos.x);
    float disy = fabs(y - this->pos.y);
    return (disx < w +this->width && disy < h+this->height);
}

void Sprite::move(float x, float y) {
    this->pos.x = x;
    this->pos.y = y;
}

Error Sprite::moveInScene(float x, float y, Scene* scene) {
    if (scene == nullptr || scene->window == nullptr) return Error(true,"scene or scene window is null");
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + this->width > scene->window->width) x = scene->window->width - this->width;
    if (y + this->height > scene->window->height) y = scene->window->height - this->height;
    this->pos.x = x;
    this->pos.y = y;
    return Error(false,"No Error");
}