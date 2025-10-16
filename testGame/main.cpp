#include <iostream>
#include <ostream>

#include "../engine/renderer.h"
#include "../engine/sprite.h"
#include "../engine/eventHandeling.h"
#include <SDL.h>

#define WIDTH 1080
#define HEIGHT 720

void onCollision(Sprite* other) {
    std::cout << "Collision with " << other->name << std::endl;
}

int main() {

    {
        Window win = Window(WIDTH, HEIGHT, "Test Game");
        Scene scene = Scene(&win, Color(0, 0, 0,win.screen->format), WIDTH, HEIGHT);
        Camera camera = Camera(0, 0, WIDTH, HEIGHT, &scene);
        Texture textureKey = Texture();
        Texture texturePable = Texture();
        EventHandeler eventHandeler;
        auto err = textureKey.load("../testGame/textures/pipe.png");
        if (err.error) {
            std::cout << "Texture load failed! "<< err.message << std::endl;
        }
        err = texturePable.load("../testGame/textures/padel.png");
        if (err.error) {
            std::cout << "Texture load failed! "<< err.message << std::endl;
        }
        int spriteW = 200;
        int spriteH = 200;
        Sprite sprite = Sprite("pipe",&textureKey, 0, 0, spriteW, spriteH);
        Sprite padle = Sprite("padel",&texturePable, 400, 400, spriteW, spriteH);
        sprite.setOnCollision(onCollision);
        eventHandeler.sprites.push_back(&sprite);
        padle.setOnCollision(onCollision);
        eventHandeler.sprites.push_back(&padle);
        while (true) {
            eventHandeler.update();
            if (eventHandeler.isQuitEvent()) break;
            if (eventHandeler.isKeyPressed(SDL_SCANCODE_W)) {
                sprite.moveInScene(sprite.pos.x,sprite.pos.y - 4,&scene);
            }
            if (eventHandeler.isKeyPressed(SDL_SCANCODE_S)) {
                sprite.moveInScene(sprite.pos.x,sprite.pos.y + 4,&scene);
            }
            if (eventHandeler.isKeyPressed(SDL_SCANCODE_A)) {
                sprite.moveInScene(sprite.pos.x - 4,sprite.pos.y ,&scene);
            }
            if (eventHandeler.isKeyPressed(SDL_SCANCODE_D)) {
                sprite.moveInScene(sprite.pos.x + 4,sprite.pos.y ,&scene);
            }
            sprite.drawSprite(&scene);
            padle.drawSprite(&scene);
            Frame frame = camera.render();
            win.update(frame, 16);
        }
    }
    exitRenderer();
    return 0;
}