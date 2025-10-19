#ifndef NGE_EVENT_HANDELING_H
#define NGE_EVENT_HANDELING_H

#include <vector>
#include "SDL/include/SDL.h"
#include "sprite.h"


class EventHandeler {
public:
    int mouseX;
    int mouseY;
    std::vector<Sprite*> sprites;
    std::function<void()> onQuit;
    EventHandeler() = default;
    ~EventHandeler() = default;
    void update();
    void updateEvents();
    void updateCollisions();
    void updateMousePosition();
    bool isQuitEvent() const { return quit; }
    bool isKeyPressed(SDL_Scancode sc) const;
    std::pair<int,int> mousPosition() const {
        return {this->mouseX, this->mouseY};
    }
    bool isMousePressed(int button) const;
    void cleanupSprites() {
        for (auto& sprite : sprites) {
            delete sprite;
        }
    }

private:
    bool quit = false;
};

#endif

