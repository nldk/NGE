
#ifndef NGE_RENDERER_H
#define NGE_RENDERER_H
#include <string>
#include <SDL2/SDL.h>

#include "color.h"

class Window {
public:
    int width;
    int height;
    std::string title;
    SDL_Window *win = nullptr;
    SDL_Surface *screen = nullptr;
    Window(int width, int height, std::string title);
    ~Window() = default;
private:
    void init();
};

class Scene {
public:
    Uint32* pixels;
    Window *window;
    Scene(Window *window);
    ~Scene();
    void update(int delay);
    void setBackgroundColor(Color color);
};

#endif