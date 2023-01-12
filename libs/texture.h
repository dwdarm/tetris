#ifndef TEXTURE_h_
#define TEXTURE_h_

#ifndef SDL_h_
    #include <SDL.h>
#endif

#ifndef SDL_IMAGE_H_
    #include <SDL_image.h>
#endif

#include <string>

class Texture {
    public:
        Texture() {}
        ~Texture();
        
        bool loadFromFile(SDL_Renderer* renderer, const std::string &path);
        SDL_Texture* getTexture() const;
        int getWidth() const;
        int getHeight() const;
        
    private:
        SDL_Texture* mTexture; 
        SDL_Surface* mSurface; 
};

#endif