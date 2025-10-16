#ifndef NGE_EVENT_HANDELING_H
#define NGE_EVENT_HANDELING_H

#include <vector>
#include <SDL2/SDL.h>
#include "sprite.h"


class EventHandeler {
public:
    std::vector<Sprite*> sprites;
    EventHandeler() = default;
    ~EventHandeler() = default;
    void update();
    void updateEvents();
    void updateCollisions();
    bool isQuitEvent() const { return quit; }
    bool isKeyPressed(SDL_Scancode sc) const;


private:
    bool quit = false;
};

#endif

