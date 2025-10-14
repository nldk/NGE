#include "../engine/renderer.h"
#include "../engine/color.h"

int main() {
    Window win = Window(800,600,"Test Window");
    Scene scene = Scene(&win);
    scene.setBackgroundColor(Color(0,0,255,win.screen->format));
    scene.update(0);
    SDL_Delay(2000);
    return 0;
}