#include "eventHandeling.h"

void EventHandeler::pump() {
    SDL_Event e;
    // Pump events to update keyboard state
    SDL_PumpEvents();
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quit = true;
        }
    }
}

bool EventHandeler::isKeyPressed(SDL_Scancode sc) const {
    int numKeys = 0;
    const Uint8* state = SDL_GetKeyboardState(&numKeys);
    if (!state) return false;
    if (sc < 0 || sc >= numKeys) return false;
    return state[sc] != 0;
}

