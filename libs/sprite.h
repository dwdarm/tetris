#ifndef SPRITE_h_
#define SPRITE_h_

#ifndef TEXTURE_h_
    #include "texture.h"
#endif

#ifndef SDL_h_
    #include <SDL.h>
#endif

class Sprite {
    public:
        
        void setTexture(Texture* texture);
        void setTextureRect(int x, int y, int width, int height);
        
        void setPosition(int x, int y);
        void move(int dx, int dy);
        void resize(int width, int height);
        void rotate(double angle);
        
        int getX() const;
        int getY() const;
        int getWidth() const;
        int getHeight() const;
        int getRotate() const;
        
        void draw(SDL_Renderer* renderer);
        
        void *data;
        
    private:
        Texture* mTexture; 
        
        int mX = 0, mY = 0;
        int mWidth = 0, mHeight = 0;
        
        double mRotate = 0;
        
        int mTX = 0, mTY = 0;
        int mTWidth = 0, mTHeight = 0;
};

#endif