#ifndef NGE_SOUND_H
#define NGE_SOUND_H
#include <SDL2/SDL_mixer.h>
#include <SDL.h>
#include "error.h"
class SoundManager {
public:
    SoundManager();
    ~SoundManager() = default;
    Error playMusic(std::string path, int loops);
    Error playSoundEffect(std::string path, int loops);
};

#endif