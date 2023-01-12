#ifndef TEXTURE_h_
#include "texture.h"

bool Texture::loadFromFile(SDL_Renderer* renderer, const std::string &path) {
    int flags = IMG_INIT_PNG;
    if(!(IMG_Init(flags) & flags)){
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }
    
    mSurface = IMG_Load(path.c_str());
    if(mSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    }
    
    mTexture = SDL_CreateTextureFromSurface(renderer, mSurface);
    if(mTexture == NULL) {
        printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    
    return true;
}

SDL_Texture* Texture::getTexture() const {
    return mTexture;
}

int Texture::getWidth() const {
    if (mSurface == NULL) {
        return -1;
    }
    
    return mSurface->w;
}

int Texture::getHeight() const {
    if (mSurface == NULL) {
        return -1;
    }
    
    return mSurface->h;
}

Texture::~Texture() {
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
    }
    
    if (mSurface != NULL) {
        SDL_FreeSurface(mSurface);
    }
}

#endif