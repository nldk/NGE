#include "eventHandeling.h"

void EventHandeler::update() {
    this->updateCollisions();
    this->updateEvents();
}

bool EventHandeler::isKeyPressed(SDL_Scancode sc) const {
    int numKeys = 0;
    const Uint8* state = SDL_GetKeyboardState(&numKeys);
    if (!state) return false;
    if (sc < 0 || sc >= numKeys) return false;
    return state[sc] != 0;
}

void EventHandeler::updateCollisions() {
    for (size_t i = 0; i < sprites.size(); i++) {
        for (size_t j = i + 1; j < sprites.size(); j++) {
            if (sprites[i]->boxCollide(sprites[j])) {
                sprites[i]->onClolission(sprites[j]);
            }
        }
    }
}
void EventHandeler::updateEvents() {
    SDL_Event e;
    SDL_PumpEvents();
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quit = true;
        }
    }
}