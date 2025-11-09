# NGE: A 2D C++ Game Engine

NGE is a lightweight 2D game engine written in C++ and built on top of the SDL library. It provides a simple, direct-to-pixels rendering pipeline, sprite management, collision detection, sound, and level serialization, making it ideal for simple games and projects.

## Features

  * **Simple Rendering Pipeline:** Includes `Window`, `Scene`, `Camera`, and `Frame` classes for managing a 2D pixel buffer and rendering it to the screen.
  * **Sprite System:** A `Sprite` class that supports `Texture` loading from image files (via `SDL_image`) or raw pixel data.
  * **Event & Collision Handling:** An `EventHandeler` class that manages keyboard/mouse input, quit events, and provides simple AABB (`boxCollide`) collision detection between sprites.
  * **Collision Callbacks:** Assign `std::function` callbacks to sprites to handle `onColide` events.
  * **Game Loop:** A `GameLoop` class to manage a stable update loop with delta-time and callback functions.
  * **Sound Management:** A `SoundManager` class that wraps `SDL_mixer` for easy playback of music (`playMusic`) and sound effects (`playSoundEffect`).
  * **Level Serialization:** A `Level` class that can save and load a scene's sprites to and from a `.json` file, using the included `json.hpp` library.
  * **Utilities:** Includes a simple `Vec2` class for 2D math, a `Color` helper, and an `Error` class for return values.
  * **Build System:** Comes with shell scripts to build and install the engine as an SDK (`nge_install_sdk.sh`) and generate new game projects (`nge_new_project.sh`).

## Dependencies

To build and run projects with NGE, you will need:

  * `cmake`
  * A C++17 compiler (like `g++` or `clang++`)
  * `SDL2`
  * `SDL2_image`
  * `SDL2_mixer`

On Debian-based systems (like Ubuntu), you can install them with:

```bash
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
```

## How to Build & Install the Engine (SDK)

The engine is designed to be built as an "SDK" (a static library, `libNGE.a`, and headers) that your game can link against.

The `nge_install_sdk.sh` script will build and install the engine. By default, it installs to `~/.local/NGE`.

```bash
# Make the script executable
chmod +x nge_install_sdk.sh

# Run the script to build and install
./nge_install_sdk.sh
```

You can specify a custom installation prefix:

```bash
./nge_install_sdk.sh --prefix /your/custom/path
```

## How to Create a New Game

Once the SDK is installed, you can generate a new game project using `nge_new_project.sh`.

```bash
# Make the script executable
chmod +x nge_new_project.sh

# Usage: ./nge_new_project.sh <ProjectName> <DestinationDirectory>
./nge_new_project.sh MyAwesomeGame ./MyAwesomeGame
```

This script creates a new directory (`MyAwesomeGame`) with a `CMakeLists.txt` and a `main.cpp` template, pre-configured to find and link against the NGE SDK.

## Core Concepts & API Example

The engine's main components are:

  * `Window`: The OS window.
  * `Scene`: The pixel buffer you draw to.
  * `Camera`: Renders the `Scene` to the `Window`.
  * `Sprite`: An object with a `Texture` and position.
  * `EventHandeler`: Manages input and collisions.
  * `GameLoop`: Runs the main update function.

Here is a minimal example based on your `main.cpp` file, adapted to use the `GameLoop` class:

```cpp
#include <iostream>
#include <nge.h> // The main engine header

#define WIDTH 800
#define HEIGHT 600

int main() {
    // 1. Initialize core components
    Window win(WIDTH, HEIGHT, "My NGE Game");
    Scene scene(&win, Color(0, 0, 0, win.screen->format), WIDTH, HEIGHT);
    Camera camera(0, 0, WIDTH, HEIGHT, &scene);
    EventHandeler eventHandeler;
    GameLoop gameLoop;

    // 2. Load assets
    Texture playerTexture;
    Error err = playerTexture.load("path/to/player.png");
    if (err.error) {
        std::cerr << "Failed to load texture: " << err.message << std::endl;
        return 1;
    }

    // 3. Create sprites
    Sprite player("player", &playerTexture, 50, 50, 64, 64);
    eventHandeler.sprites.push_back(&player); // Add to event handler for collisions

    // 4. Set the main game loop callback
    gameLoop.addUpdatFunc([&](float delta) {
        // --- Update game logic ---
        // 'delta' is the time in seconds since the last frame
        float moveSpeed = 200.0f;

        if (eventHandeler.isKeyPressed(SDL_SCANCODE_D)) {
            player.moveInScene(player.pos.x + moveSpeed * delta, player.pos.y, &scene);
        }
        if (eventHandeler.isKeyPressed(SDL_SCANCODE_A)) {
            player.moveInScene(player.pos.x - moveSpeed * delta, player.pos.y, &scene);
        }

        // Handle events (input, collisions)
        eventHandeler.update(); 

        // --- Render the scene ---
        scene.resetScreen();       // Clear screen to background color
        player.drawSprite(&scene); // Draw sprites to the scene buffer

        Frame frame = camera.render(); // Get the final frame from the camera
        win.update(frame, 16);         // Draw the frame to the window
    });

    // 5. Handle the quit event
    eventHandeler.onQuit = [&]() {
        gameLoop.stop();
        exitRenderer();
    };

    // 6. Start the game loop
    gameLoop.start();

    return 0;
}
```

## Project Structure (Key Classes)

  * `nge.h`: Main include file that pulls in all other engine components.
  * `renderer.h`: Contains `Window`, `Scene`, `Camera`, `Frame`, and `GameLoop` for the rendering pipeline.
  * `sprite.h`: Contains `Sprite` and `Texture`, the primary renderable objects.
  * `eventHandeling.h`: Contains `EventHandeler` for managing input, collisions, and the quit event.
  * `level.h`: Contains `Level` for saving/loading sprite layouts to/from JSON.
  * `sound.h`: Contains `SoundManager` for playing music and sound effects.
  * `vec2.h`: A `Vec2` class for 2D vector math.
  * `color.h`: A `Color` wrapper for `SDL_PixelFormat`.
  * `button.h`: A `Button` class that inherits from `Sprite`.
  * `text.h`: (Placeholder for text rendering).
  * `json.hpp`: Third-party library for JSON serialization (Niels Lohmann).
  * `miniz.h` / `miniz.c`: Third-party library for ZIP compression.
