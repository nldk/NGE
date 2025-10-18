#ifndef NGE_SOUND_H
#define NGE_SOUND_H
#include "nge.h"
#include "error.h"
class SoundManager {
public:
    SoundManager();
    ~SoundManager() = default;
    Error playMusic(std::string path, int loops);
    Error playSoundEffect(std::string path, int loops);
};

#endif