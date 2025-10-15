#ifndef NGE_RENDERER_H
#define NGE_RENDERER_H
#include <string>
#include <SDL2/SDL.h>
#include "vec2.h"
#include "color.h"

void exitRenderer();

class Frame {
public:
    Uint32* pixels = nullptr;
    Frame() noexcept = default;
    ~Frame() {
        delete[] pixels;
        pixels = nullptr;
    };
    Frame(const Frame&) = delete;
    Frame& operator=(const Frame&) = delete;
    Frame(Frame&& other) noexcept : pixels(other.pixels) {
        other.pixels = nullptr;
    }
    Frame& operator=(Frame&& other) noexcept {
        if (this != &other) {
            delete[] pixels;
            pixels = other.pixels;
            other.pixels = nullptr;
        }
        return *this;
    }
};

class Window {
public:
    int width;
    int height;
    std::string title;
    SDL_Window *win = nullptr;
    SDL_Surface *screen = nullptr;
    void update(const Frame& frame,int delay) const;
    Window(int width, int height, std::string title);
    ~Window() {
        SDL_DestroyWindow(this->win);
        this->win = nullptr;
        this->screen = nullptr;
    };
private:
    void init();
};

class Scene {
public:
    Uint32 background;
    Uint32* pixels;
    Window *window;
    Scene(Window *window, Color background,int width,int height);
    ~Scene();
    void setBackgroundColor(Color color);
    void resetScreen();
};

class Camera {
public:
    Vec2 pos;
    Vec2 size;
    Scene* scene;
    Camera(float x, float y, float width, float height,Scene* scene);
    ~Camera() = default;
    Frame render() const;

};
#endif