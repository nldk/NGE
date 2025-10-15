#ifndef NGE_EVENT_HANDELING_H
#define NGE_EVENT_HANDELING_H

#include <SDL2/SDL.h>

class EventHandeler {
public:
    EventHandeler() = default;
    ~EventHandeler() = default;

    // Poll and pump SDL events; updates internal quit flag
    void pump();

    // True if a quit event was received (window close or ESC)
    bool isQuitEvent() const { return quit; }

    // Returns whether a given scancode is currently pressed
    bool isKeyPressed(SDL_Scancode sc) const;

private:
    bool quit = false;
};

#endif // NGE_EVENT_HANDELING_H

