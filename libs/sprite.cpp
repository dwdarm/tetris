#ifndef SPRITE_h_
#include "sprite.h"

void Sprite::setTexture(Texture* texture) {
    mTexture = texture;
    mTX = 0;
    mTY = 0;
    mWidth = mTWidth = texture->getWidth();
    mHeight = mTHeight = texture->getHeight();
    
}

void Sprite::setTextureRect(int x, int y, int width, int height) {
    mTX = x % mTexture->getWidth();
    mTY = y;
    mTWidth = width;
    mTHeight = height;
}

void Sprite::setPosition(int x, int y) {
    mX = x;
    mY = y;
}

void Sprite::move(int dx, int dy) {
    mX += dx;
    mY += dy;
}

void Sprite::rotate(double angle) {
    mRotate += angle;
}

void Sprite::resize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

int Sprite::getX() const {
    return mX;
}

int Sprite::getY() const {
    return mY;
}

int Sprite::getWidth() const {
    return mWidth;
}

int Sprite::getHeight() const {
    return mHeight;
}

int Sprite::getRotate() const {
    return mRotate;
}

void Sprite::draw(SDL_Renderer* renderer) {
    SDL_Rect quad = { mX, mY, mWidth, mHeight };
    SDL_Rect clip = { mTX, mTY, mTWidth, mTHeight };
    
    if ((mTX + mTWidth) > mTexture->getWidth()) {
        Uint32 format;
        SDL_QueryTexture(mTexture->getTexture(), &format, NULL, NULL, NULL);
        SDL_Texture* temp = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, mWidth, mHeight);
        SDL_SetRenderTarget(renderer, temp);
        
        quad.w = clip.w = mTWidth - mTX;
        SDL_RenderCopy(renderer, mTexture->getTexture(), &clip, &quad);
        
        clip.x = 0;
        quad.x = mTWidth - mTX;
        quad.w = clip.w = mTX;
        SDL_RenderCopy(renderer, mTexture->getTexture(), &clip, &quad);
        
        quad.x = mX;
        quad.w = mWidth;
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopyEx(renderer, temp, NULL, &quad, mRotate, NULL, SDL_FLIP_NONE);
        SDL_DestroyTexture(temp);
        
    } else {
        SDL_RenderCopyEx(renderer, mTexture->getTexture(), &clip, &quad, mRotate, NULL, SDL_FLIP_NONE);
    }
}

#endif