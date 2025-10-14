#include "renderer.h"

#include <cstring>

Window::Window(int width, int height, std::string title) {
    this->width = width;
    this->height = height;
    this->title = title;
    init();
}
void Window::init() {
    SDL_Init(SDL_INIT_VIDEO);
    this->win = SDL_CreateWindow(title.c_str(),
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           width, height, 0);
    this->screen = SDL_GetWindowSurface(win);
}

Scene::Scene(Window *window) {
    this->window = window;
    this->pixels = new Uint32[window->width * window->height];
}
Scene::~Scene() {
    delete[] pixels;
}
void Scene::update(int delay) {
    SDL_LockSurface(this->window->screen);
    Uint32 *pixels = (Uint32*) this->window->screen->pixels;
    std::memcpy(pixels, this->pixels, window->width * window->height * sizeof(Uint32));
    SDL_UnlockSurface(this->window->screen);
    SDL_UpdateWindowSurface(this->window->win);
    SDL_Delay(delay);
}
void Scene::setBackgroundColor(Color color) {
    for (int y = 0; y < this->window->height; y++) {
        for (int x = 0; x < this->window->width; x++) {
            this->pixels[y * this->window->width + x] = color.get();
        }
    }
}