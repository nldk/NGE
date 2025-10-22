#include "sound.h"

SoundManager::SoundManager() {
    if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO)) {
        SDL_Init(SDL_INIT_AUDIO);
    }
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

Error SoundManager::playMusic(std::string path, int loops) {
    Mix_Music *music = Mix_LoadMUS(path.c_str());
    if (!music) {
        return Error(true, "Failed to load sound: " + static_cast<std::string>(Mix_GetError()));
    }
    Mix_PlayMusic(music, loops);
    Mix_ChannelFinished([](int chan) {
        Mix_Chunk *finishedChunk = Mix_GetChunk(chan);
        if (finishedChunk) {
            Mix_FreeChunk(finishedChunk);
        }
    });
    return Error(false, "No error");
}

Error SoundManager::playSoundEffect(std::string path, int loops) {
    Mix_Chunk *soundEffect = Mix_LoadWAV(path.c_str());
    if (!soundEffect) {
        return Error(true, "Failed to load sound: " + static_cast<std::string>(Mix_GetError()));
    }
    int channel = Mix_PlayChannel(-1, soundEffect, loops);
    if (channel == -1) {
        Mix_FreeChunk(soundEffect);;
        return Error(true, "Failed to play sound: " + static_cast<std::string>(Mix_GetError()));
    }
    return Error(false, "No error");
}
