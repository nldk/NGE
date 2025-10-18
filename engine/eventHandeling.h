#ifndef NGE_EVENT_HANDELING_H
#define NGE_EVENT_HANDELING_H

#include <vector>
#include "nge.h"
#include "sprite.h"


class EventHandeler {
public:
    int mouseX = 0;
    int mouseY = 0;
    std::vector<Sprite*> sprites;
    std::function<void()> onQuit;
    EventHandeler() = default;
    ~EventHandeler() = default;
    void update();
    void updateEvents();
    void updateCollisions();
    void updateMousePosition();
    [[nodiscard]] bool isQuitEvent() const { return quit; }
    [[nodiscard]] bool isKeyPressed(SDL_Scancode sc) const;
    [[nodiscard]] std::pair<int,int> mousPosition() const {
        return {this->mouseX, this->mouseY};
    }
    [[nodiscard]] bool isMousePressed(int button) const;
    void cleanupSprites() {
        for (auto& sprite : sprites) {
            delete sprite;
        }
    }

private:
    bool quit = false;
};

#endif
