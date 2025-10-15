#include "renderer.h"

#include <algorithm>
#include <cstring>
#include <SDL_image.h>

Window::Window(int width, int height, std::string title) {
    this->width = width;
    this->height = height;
    this->title = std::move(title);
    init();
}

void Window::init() {
    if (!(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO)) {
        SDL_Init(SDL_INIT_VIDEO);
    }
    this->win = SDL_CreateWindow(title.c_str(),
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           width, height, 0);
    this->screen = SDL_GetWindowSurface(win);
}

Scene::Scene(Window *window, Color background,int width,int height) {
    this->window = window;
    this->pixels = new Uint32[width * height];
    this->background = background.get();
}

Scene::~Scene() {
    delete[] pixels;
}

void Window::update(const Frame& frame,int delay) const {
    SDL_LockSurface(this->screen);
    auto* pixels = (Uint32*) this->screen->pixels;
    std::memcpy(pixels, frame.pixels, this->width * this->height * sizeof(Uint32));
    SDL_UnlockSurface(this->screen);
    SDL_UpdateWindowSurface(this->win);
    SDL_Delay(delay);
}

void Scene::setBackgroundColor(Color color) {
    this->background = color.get();
    for (int y = 0; y < this->window->height; y++) {
        for (int x = 0; x < this->window->width; x++) {
            this->pixels[y * this->window->width + x] = color.get();
        }
    }
}

void Scene::resetScreen() {
    this->setBackgroundColor(Color(this->background));
}

Camera::Camera(float x, float y, float width, float height,Scene* scene) {
    this->pos = Vec2(x,y);
    this->size = Vec2(width,height);
    this->scene = scene;
}
Frame Camera::render() const {
    Frame frame;
    int winW = this->scene->window->width;
    int winH = this->scene->window->height;
    int camW = (int)this->size.x;
    int camH = (int)this->size.y;
    frame.pixels = new Uint32[winW * winH];
    for (int y = 0; y < winH; ++y) {
        for (int x = 0; x < winW; ++x) {
            float u = (float)x / (float)winW;
            float v = (float)y / (float)winH;
            int sceneX = (int)(this->pos.x + u * camW);
            int sceneY = (int)(this->pos.y + v * camH);
            sceneX = std::clamp(sceneX, 0, winW - 1);
            sceneY = std::clamp(sceneY, 0, winH - 1);
            frame.pixels[y * winW + x] =
                this->scene->pixels[sceneY * winW + sceneX];
        }
    }

    this->scene->resetScreen();
    return frame;
}

void exitRenderer() {
    IMG_Quit();
    SDL_Quit();
}
