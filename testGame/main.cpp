#include <iostream>
#include <ostream>
#include <algorithm>

#include "../engine/renderer.h"
#include "../engine/sprite.h"
#include "../engine/eventHandeling.h"
#include  "../engine/level.h"
#include "../engine/sound.h"

// SDL is included transitively via engine headers (nge.h). No direct include needed here.

#define WIDTH 1080
#define HEIGHT 720

bool running = true;

void onExit() {
    running = false;
}

void onCollision(Sprite *other) {
    std::cout << "Collision with " << other->name << std::endl;
}

void simpleGame() {
    Window win = Window(WIDTH, HEIGHT, "Test Game");
    Scene scene = Scene(&win, Color(0, 0, 0, win.screen->format), WIDTH, HEIGHT);
    Camera camera = Camera(0, 0, WIDTH, HEIGHT, &scene);
    Texture textureKey = Texture();
    Texture texturePable = Texture();
    EventHandeler eventHandeler;
    auto err = textureKey.load("../testGame/textures/pipe.png");
    if (err.error) {
        std::cout << "Texture load failed! " << err.message << std::endl;
    }
    err = texturePable.load("../testGame/textures/padel.png");
    if (err.error) {
        std::cout << "Texture load failed! " << err.message << std::endl;
    }
    int spriteW = 200;
    int spriteH = 200;
    Sprite sprite = Sprite("pipe", &textureKey, 0, 0, spriteW, spriteH);
    Sprite padle = Sprite("padel", &texturePable, 400, 400, spriteW, spriteH);
    sprite.setOnCollision(onCollision);
    eventHandeler.sprites.push_back(&sprite);
    padle.setOnCollision(onCollision);
    eventHandeler.sprites.push_back(&padle);
    eventHandeler.onQuit = onExit;
    while (running) {
        eventHandeler.update();
        //if (eventHandeler.isQuitEvent()) break;
        if (eventHandeler.isKeyPressed(SDL_SCANCODE_W)) {
            sprite.moveInScene(sprite.pos.x, sprite.pos.y - 4, &scene);
        }
        if (eventHandeler.isKeyPressed(SDL_SCANCODE_S)) {
            sprite.moveInScene(sprite.pos.x, sprite.pos.y + 4, &scene);
        }
        if (eventHandeler.isKeyPressed(SDL_SCANCODE_A)) {
            sprite.moveInScene(sprite.pos.x - 4, sprite.pos.y, &scene);
        }
        if (eventHandeler.isKeyPressed(SDL_SCANCODE_D)) {
            sprite.moveInScene(sprite.pos.x + 4, sprite.pos.y, &scene);
        }
        sprite.drawSprite(&scene);
        padle.drawSprite(&scene);
        Frame frame = camera.render();
        win.update(frame, 16);
    }
}

void leveltestSave() {
    {
        Window win = Window(WIDTH, HEIGHT, "Test Game");
        Scene scene = Scene(&win, Color(0, 0, 0, win.screen->format), WIDTH, HEIGHT);
        Texture textureKey = Texture();
        Texture texturePable = Texture();
        Camera camera = Camera(0, 0, WIDTH, HEIGHT, &scene);
        auto err = textureKey.load("../testGame/textures/pipe.png");
        if (err.error) {
            std::cout << "Texture load failed! " << err.message << std::endl;
        }
        err = texturePable.load("../testGame/textures/padel.png");
        if (err.error) {
            std::cout << "Texture load failed! " << err.message << std::endl;
        }
        std::cout << textureKey.path << std::endl;
        std::vector<Sprite *> sprites;
        Sprite sprite = Sprite("pipe", &textureKey, 0, 0, 200, 200);
        sprites.push_back(&sprite);
        Sprite sprite2 = Sprite("key", &texturePable, 300, 300, 200, 200);
        sprites.push_back(&sprite2);
        Level level = Level(sprites, "testLevel.json");
        level.addSprite(&sprite);
        level.render(&scene);
        win.update(camera.render(), 2000);
        level.saveLevel();
        SDL_Delay(2000);
    }
}

void leveltestLoad() {
    {
        Window win = Window(WIDTH, HEIGHT, "Test Game");
        Scene scene = Scene(&win, Color(0, 0, 0, win.screen->format), WIDTH, HEIGHT);
        Texture textureKey = Texture();
        Camera camera = Camera(0, 0, WIDTH, HEIGHT, &scene);
        auto err = textureKey.load("../testGame/textures/pipe.png");
        if (err.error) {
            std::cout << "Texture load failed! " << err.message << std::endl;
        }
        std::cout << textureKey.path << std::endl;
        std::vector<Sprite *> sprites;
        Sprite sprite = Sprite("pipe", &textureKey, 0, 0, 200, 200);
        sprites.push_back(&sprite);
        Level level = Level(sprites, "testLevel.json");
        level.loadLevel();
        level.render(&scene);
        win.update(camera.render(), 2000);
        level.saveLevel();
        SDL_Delay(2000);
    }
}

void sandBoxGame() { {
        int name = 0;
        bool running = true;
        Window win = Window(WIDTH, HEIGHT, "sandy Game");
        Scene scene = Scene(&win, Color(0, 0, 0, win.screen->format), WIDTH, HEIGHT);
        Camera camera = Camera(0, 0, WIDTH, HEIGHT, &scene);
        std::vector<Sprite *> sprites;
        Level level = Level(sprites, "sandBoxLevel.json");
        Texture textureSand = Texture();
        EventHandeler eventHandeler = EventHandeler();
        eventHandeler.onQuit = [&running,&name]() {
            running = false;
            std::cout << name << std::endl;
        };
        auto err = textureSand.load("../testGame/textures/padel.png");
        if (err.error) {
            std::cout << "Texture load failed! " << err.message << std::endl;
        }
        while (running) {
            eventHandeler.update();
            if (eventHandeler.isMousePressed(1)) {
                Sprite *sprite = new Sprite("sprite " + name, &textureSand, eventHandeler.mouseX - 25,
                                            eventHandeler.mouseY - 25, 50, 50);
                level.addSprite(sprite);
                name++;
            }
            level.render(&scene);
            Frame frame = camera.render();
            win.update(frame, 8);
        }
        eventHandeler.cleanupSprites();
    }
    exitRenderer();
}

void photoViewer(std::string path) {
    //std::cout << path << std::endl;
    bool running = true;
    Texture texturePhoto = Texture();
    auto err = texturePhoto.load(path);
    if (err.error) {
        std::cout << "Texture load failed! " << err.message << std::endl;
    }
    float aspect = (float) texturePhoto.width / (float) texturePhoto.height;
    int width = texturePhoto.width;
    int height = texturePhoto.height;
    double scaleX = (double) 3000 / width;
    double scaleY = (double) 2000 / height;
    double scale = std::min(scaleX, scaleY); // keep aspect ratio
    int newWidth = static_cast<int>(width * scale);
    int newHeight = static_cast<int>(height * scale);

    Window win = Window(newWidth, newHeight, "Photo Viewer");
    Scene scene = Scene(&win, Color(0, 0, 0, win.screen->format), newWidth, newHeight);
    Camera camera = Camera(0, 0, newWidth, newHeight, &scene);
    EventHandeler eventHandeler = EventHandeler();
    eventHandeler.onQuit = [&running]() {
        running = false;
    };

    Sprite sprite = Sprite("photo", &texturePhoto, 0, 0, newWidth, newHeight);
    sprite.drawSprite(&scene);
    Frame frame = camera.render();
    win.update(frame, 0);
    while (running) {
        eventHandeler.update();
        SDL_Delay(50);
    }
}

void soundTest() {
    SoundManager soundManager = SoundManager();
    auto err = soundManager.playMusic("../testGame/sound/blue.mp3", 0);
    if (err.error) {
        std::cout << "Sound play failed! " << err.message << std::endl;
    }
    SDL_Delay(10000);
}

void col() {
    bool running = true;
    bool hitTheGround = false;
    Frame frame = Frame();
    EventHandeler eventHandeler = EventHandeler();
    Window win = Window(WIDTH, HEIGHT, "Col");
    Scene scene = Scene(&win, Color(0, 0, 0, win.screen->format), WIDTH, HEIGHT);
    Camera camera = Camera(0, 0, WIDTH, HEIGHT, &scene);
    Texture textureA = Texture();
    textureA.load("../testGame/textures/pipe.png");
    Texture textureB = Texture();
    textureB.load("../testGame/textures/padel.png");
    Sprite spriteA = Sprite("A", &textureA, 150, 50, 50, 50);
    Sprite spriteB = Sprite("B", &textureB, 150, 150, 200, 50);
    spriteA.onColide = [&hitTheGround](Sprite* other) {
        if (other->name == "B") {
            hitTheGround = true;
        }
    };
    eventHandeler.onQuit = [&running]() {
        running = false;
        exitRenderer();
    };
    eventHandeler.sprites = {&spriteA, &spriteB};
    while (running) {
        eventHandeler.update();
        eventHandeler.updateCollisions();
        if (!hitTheGround) {
            spriteA.moveInScene(spriteA.pos.x, spriteA.pos.y + 4, &scene);
        }else {
            hitTheGround = false;
        }
        if (eventHandeler.isKeyPressed(SDL_SCANCODE_A)) {
            spriteA.moveInScene(spriteA.pos.x - 4, spriteA.pos.y, &scene);
        }
        if (eventHandeler.isKeyPressed(SDL_SCANCODE_D)) {
            spriteA.moveInScene(spriteA.pos.x + 4, spriteA.pos.y, &scene);
        }
        spriteA.drawSprite(&scene);
        spriteB.drawSprite(&scene);
        frame = camera.render();
        win.update(frame, 16);
    }
}

int main(int argc, char *argv[]) {
    col();
    //soundTest();
    /*
    if (argc < 2) {
        std::cout << "Usage: photoViewer <image_path>" << std::endl;
        return 1;
    }
    std::cout << argv[1] << std::endl;
    photoViewer(argv[1]);
    */
    //sandBoxGame();
    //leveltestSave();
    //leveltestLoad();
    return 0;
}
