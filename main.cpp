/*
    Written by Fajar Dwi Darmanto
*/

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <vector>
#include <memory>

#include "libs/texture.h"
#include "libs/sprite.h"

#define TILE_SIZE 32
#define TILE_COUNT 6

enum {
    I = 0,
    L,
    RL,
    SQ,
    S,
    RS,
    T
} BLOCK_SHAPES;

#define SHAPE_COUNT 7
#define BLOCK_SIZE 24
#define STACK_WIDTH 10
#define STACK_HEIGHT 20

const int STACK_FRAME_WIDTH = STACK_WIDTH * BLOCK_SIZE;
const int STACK_FRAME_HEIGHT = STACK_HEIGHT * BLOCK_SIZE;

const int SCREEN_WIDTH = STACK_FRAME_WIDTH + (10 * BLOCK_SIZE);
const int SCREEN_HEIGHT = STACK_FRAME_HEIGHT  + (2 * BLOCK_SIZE);

int SHAPES[SHAPE_COUNT][8] = {
    {1,1,1,1,0,0,0,0}, // I
    {1,1,1,0,0,0,1,0}, // L
    {0,0,1,0,1,1,1,0}, // RL
    {1,1,0,0,1,1,0,0}, // SQ
    {1,1,0,0,0,1,1,0}, // S
    {0,1,1,0,1,1,0,0}, // RS
    {1,1,1,0,0,1,0,0}, // T
};

class Score {
    public:
        Score(int x, int y) {
            mX = x;
            mY = y;
        }
        
        void draw(SDL_Renderer* renderer) {
            boxRGBA(renderer, mX + (7 * BLOCK_SIZE), mY, mX, mY + (4 * BLOCK_SIZE), 0, 0, 0, 0xFF);
            stringRGBA(renderer, mX + BLOCK_SIZE, mY + BLOCK_SIZE, "Score", 0xFF, 0xFF, 0xFF, 0xFF);
            stringRGBA(renderer, mX + BLOCK_SIZE, mY + (2 * BLOCK_SIZE), std::to_string(mScore).c_str(), 0xFF, 0xFF, 0xFF, 0xFF);
        }
        
        void increment(int val) {
            mScore += val;
        } 
        
        void reser() {
            mScore = 0;
        }
    
    private:
        int mX = 0, mY = 0, mScore = 0;
};

class Stack {
    public:
    
        Stack(SDL_Renderer* renderer, const std::string &texturePath, int x, int y) {
            mTexture.loadFromFile(renderer, texturePath);
            mX = x;
            mY = y;
        }
    
        void put(int val, int x, int y) {
            mStack[y][x] = val;
        }
    
        int getStackState(int x, int y) {
            return mStack[y][x];
        }
        
        int getX() {
            return mX;
        }
        
        int getY() {
            return mY;
        }
        
        const int *getStack() {
            return &mStack[0][0];
        }
        
        int update() {
            int res = 0;

            if (mCutStackIndex.size() == 0) {
                res = _cutStack();
            }

            if (mJoinStack) {
                _joinStack();
            }

            return res;
        }
        
        void draw(SDL_Renderer* renderer) {
            Uint32 format;
            SDL_QueryTexture(mTexture.getTexture(), &format, NULL, NULL, NULL);
            SDL_Texture *stackTexture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, STACK_WIDTH * BLOCK_SIZE, STACK_HEIGHT * BLOCK_SIZE);
            SDL_SetRenderTarget(renderer, stackTexture);
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(renderer);
            
            for (int y = 0; y < STACK_HEIGHT; y++) {
                bool cutX = true;
                for (int x = 0; x < STACK_WIDTH; x++) {
                    
                    if (mStack[y][x] > 0) {

                        SDL_Rect clip = { (mStack[y][x] - 1) * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
                        SDL_Rect quad = { (x * BLOCK_SIZE), (y * BLOCK_SIZE), BLOCK_SIZE, BLOCK_SIZE };

                        if ((mCutStackIndex.size() > 0) && cutX) {
                            int offset = 0;
                            for (int cutY : mCutStackIndex) {
                                offset++;

                                if (cutY != y) {
                                    SDL_RenderCopy(renderer, mTexture.getTexture(), &clip, &quad);
                                } else {
                                    mStack[y][x] = -1;
                                    cutX = false;

                                    if ((x == (STACK_WIDTH - 1)) && (offset == mCutStackIndex.size())) {
                                        mCutStackIndex.clear();
                                        mJoinStack = true;
                                    }
                                }

                            }

                        } else {
                            SDL_RenderCopy(renderer, mTexture.getTexture(), &clip, &quad);
                        }

                    }

                }
            }
            
            SDL_SetRenderTarget(renderer, NULL);
            SDL_Rect quad = { mX, mY, STACK_FRAME_WIDTH, STACK_FRAME_HEIGHT };
            SDL_RenderCopy(renderer, stackTexture, NULL, &quad);
            SDL_DestroyTexture(stackTexture);
        }
    
    private:
    
        int _cutStack() {
            int total = 0;

            for (int y = 0; y < STACK_HEIGHT; y++) {
                bool cut = true;
                for (int x = 0; x < STACK_WIDTH; x++) {
                    if (mStack[y][x] <= 0) {
                        cut = false;
                        break;
                    }
                }
                
                if (cut == true) {
                    total++;
                    mCutStackIndex.push_back(y);
                    /*
                    for (int z = 0; z < STACK_WIDTH; z++) {
                        mStack[y][z] = -1;
                    }*/
                }
            }
            
            return total;
        }
        
        void _joinStack() {
            bool finish = false;
            
            while(!finish) {
                finish = true;
                
                for (int y = STACK_HEIGHT - 1; y >= 0; y--) {
                    bool shift = true;
                    for (int x = 0; x < STACK_WIDTH; x++) {
                        if (mStack[y][x] != -1) {
                            shift = false;
                            break;
                        }
                    }
                
                    if (shift == true) {
                        finish = false;
                        for (int p = y; p > 0; p--) {
                            for (int q = 0; q < STACK_WIDTH; q++) {
                                mStack[p][q] = mStack[p-1][q]; 
                            }
                        }
                        
                        break;
                    }
                }
            }

            mJoinStack = false;

        }
    
        Texture mTexture;
        
        int mStack[STACK_HEIGHT][STACK_WIDTH] = {0};
        int mX = 0, mY=0;
        std::vector<int> mCutStackIndex;
        bool mJoinStack = false;
};

class Block {
    public:
    
        void reload(Texture* texture, int x, int y) {
            mTexture = texture;
            mTexNum = rand() % TILE_COUNT;
            mShape = rand() % SHAPE_COUNT;
            
            mSprites.clear();
            
            for (int i = 0; i < 8; i++) {
                if (SHAPES[mShape][i] == 1) {
                    std::shared_ptr<Sprite> sprite(new Sprite());
                    sprite->setTexture(mTexture);
                    sprite->resize(BLOCK_SIZE, BLOCK_SIZE);
                    sprite->setTextureRect(mTexNum * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE);
                    sprite->setPosition(((i % 4) * BLOCK_SIZE) + y, i < 4 ? x : x + BLOCK_SIZE);
                    mSprites.push_back(sprite);
                }
            }
        }
        
        void down() {
            for(std::shared_ptr<Sprite> sp : mSprites) {
                sp->move(0, BLOCK_SIZE);
            }
        }
        
        void left() {
            for(std::shared_ptr<Sprite> sp : mSprites) {
                sp->move(-1 * BLOCK_SIZE, 0);
            }
        }
        
        void right() {
            for(std::shared_ptr<Sprite> sp : mSprites) {
                sp->move(BLOCK_SIZE, 0);
            }
        }
        
        void getRotatePoints(std::vector<SDL_Point> &points) {
            std::shared_ptr<Sprite> center = mSprites[1];
            
            for (std::shared_ptr<Sprite> sp : mSprites) {
                if (sp.get() != center.get()) {
                    points.push_back({
                        -1*(sp->getY() - center->getY()) + center->getX(),
                        sp->getX() - center->getX() + center->getY()
                    });
                } else {
                    points.push_back({ sp->getX(), sp->getY() });
                }
            }
        }
        
        void setPoints(std::vector<SDL_Point> &points) {
            int i = 0;
            for (std::shared_ptr<Sprite> sp : mSprites) {
                sp->setPosition(points[i].x, points[i].y);
                i++;
            }
        }
        
        void draw(SDL_Renderer* renderer) {
            for (std::shared_ptr<Sprite> sp : mSprites) {
                sp->draw(renderer);
            }
        }
        
        int getTexNum() {
            return mTexNum;
        }
        
        const std::vector<std::shared_ptr<Sprite>>& getSprites() const {
            return mSprites;
        }
    
    private:
        Texture* mTexture;
        std::vector<std::shared_ptr<Sprite>> mSprites;
        int mShape, mTexNum = 0;
};

class Blocks {
    public:
    
        Blocks(SDL_Renderer* renderer, const std::string &texturePath, Stack* stack, Score *score) {
            mTexture.loadFromFile(renderer, texturePath);
            mStack = stack;
            mScore = score;
            mBlock.reload(&mTexture, mStack->getX(), mStack->getX());
        }
        
        void down() {
            for(std::shared_ptr<Sprite> sp : mBlock.getSprites()) {
                if (sp->getY() - mStack->getY() >= 0) {
                    if ((sp->getY() + BLOCK_SIZE == (mStack->getY() + STACK_FRAME_HEIGHT)) ||
                        *(mStack->getStack() + (((sp->getY() - mStack->getY())/BLOCK_SIZE) + 1)*STACK_WIDTH + ((sp->getX() - mStack->getX())/BLOCK_SIZE)) - 1 >= 0) {
                     
                        for(std::shared_ptr<Sprite> sp : mBlock.getSprites()) {
                            mStack->put(mBlock.getTexNum() + 1, (sp->getX() - mStack->getX())/BLOCK_SIZE, (sp->getY() - mStack->getY())/BLOCK_SIZE);
                        }
                    
                        return mBlock.reload(&mTexture, mStack->getX(), mStack->getX());
                    }
                }
            }
            
            mBlock.down();
        }
        
        void left() {
            for(std::shared_ptr<Sprite> sp : mBlock.getSprites()) {
                if (sp->getX() + (-1 * BLOCK_SIZE) < mStack->getX()) {
                    return;
                }
                
                if (sp->getY() - mStack->getY() >= 0) {
                    if (*(mStack->getStack() + ((sp->getY() - mStack->getY())/BLOCK_SIZE)*STACK_WIDTH + ((sp->getX() - mStack->getX())/BLOCK_SIZE) - 1) - 1 >= 0) {
                        return;
                    }
                }
            }
            
            mBlock.left();
        }
        
        void right() {
            for(std::shared_ptr<Sprite> sp : mBlock.getSprites()) {
                if (sp->getX() + BLOCK_SIZE > (mStack->getX() + STACK_FRAME_WIDTH - BLOCK_SIZE)) {
                    return;
                }
                
                if (sp->getY() - mStack->getY() >= 0) {
                    if (*(mStack->getStack() + ((sp->getY() - mStack->getY())/BLOCK_SIZE)*STACK_WIDTH + ((sp->getX() - mStack->getX())/BLOCK_SIZE) + 1) - 1 >= 0) {
                        return;
                    }
                }
            }
            
            mBlock.right();
        }
        
        void rotate() {
            std::shared_ptr<Sprite> center = mBlock.getSprites()[1];
            std::vector<SDL_Point> points;
            
            mBlock.getRotatePoints(points);
            
            for (SDL_Point point : points) {
                if ((point.y >= 0)) {
                    if (*(mStack->getStack() + ((point.y + mStack->getY())/BLOCK_SIZE)*STACK_WIDTH + ((point.x - mStack->getX())/BLOCK_SIZE)) - 1 >= 0) {
                        return;
                    }
                }
            }
            
            bool finish = true;
            bool shiftRight = true;
            
            while (true) {
                
                for (int j = 0; j < points.size(); j++) {
                    if (points[j].x < mStack->getX()) {
                        shiftRight = true;
                        finish = false;
                    } else if (points[j].x > (mStack->getX() + STACK_FRAME_WIDTH - BLOCK_SIZE)) {
                        shiftRight = false;
                        finish = false;
                    }
                    
                    if ((points[j].y >= 0)) {
                        if (*(mStack->getStack() + (points[j].y/BLOCK_SIZE)*STACK_WIDTH + (points[j].x/BLOCK_SIZE)) - 1 >= 0) {
                            return;
                        }
                    }
                }  
                
                if (finish == true) {
                    break;
                }

                for (int j = 0; j < points.size(); j++) {
                    points[j].x += shiftRight == true ? BLOCK_SIZE : -1 * BLOCK_SIZE;
                }
                
                finish = true;
            }
            
            mBlock.setPoints(points);
        }
        
        void draw(SDL_Renderer* renderer) {
            mBlock.draw(renderer);
        }
        
        void tick(Uint32 interval) {
            static Uint32 blockTick = 0;
            static Uint32 stackTick = 0;
            
            blockTick += interval;
            stackTick += interval;
            
            if (stackTick >= 50) {
                int res = mStack->update();
                if (res > 0) {
                    mScore->increment(res * 100);
                }
                
                stackTick = 0;
            }
            
            if (blockTick >= 750) {
                down();
                blockTick = 0;
            }
            
        }
    
    private:
        Texture mTexture;
        Block mBlock;
        Stack* mStack;
        Score* mScore;
};

Uint32 tick_cb(Uint32 interval, void* data) {
    Blocks* blocks = (Blocks*)data;
    blocks->tick(interval);
    
    return interval;
}

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    } 
    
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;
    }
    
    srand(time(0));
    
    Stack stack(renderer, "assets/block.png", BLOCK_SIZE, BLOCK_SIZE);
    Score score(STACK_FRAME_WIDTH + (2 * BLOCK_SIZE), BLOCK_SIZE);
    Blocks blocks(renderer, "assets/block.png", &stack, &score);
    
    bool quit = false;
    SDL_Event e;
    
    SDL_TimerID timer = SDL_AddTimer(17, tick_cb, (void*)&blocks);
    
    while(!quit) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            } else if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                    case SDLK_DOWN:
                        blocks.down();
                        break;
                    case SDLK_LEFT:
                        blocks.left();
                        break;
                    case SDLK_RIGHT:
                        blocks.right();
                        break;
                    case SDLK_UP:
                        blocks.rotate();
                        break;
                    default:
                        break;
                }
            } 
        }

        SDL_SetRenderDrawColor(renderer, 0x37, 0x30, 0xA3, 0xFF);
        SDL_RenderClear(renderer);
        stack.draw(renderer);
        blocks.draw(renderer);
        score.draw(renderer);
        SDL_RenderPresent(renderer);
    }
    
    SDL_RemoveTimer(timer);

    return 0;
}