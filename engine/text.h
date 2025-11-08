#ifndef NGE_TEXT_H
#define NGE_TEXT_H

#include <functional>
#include <unordered_map>
#include "sprite.h"
class Fond:Sprite{
public:
    std::string text;
    std::unordered_map<std::string,Texture> fonts;
    ~Fond(){
        for (auto& t : fonts){
            delete &t.first;
        }
    };
};


#endif