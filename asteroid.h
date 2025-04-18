#ifndef _ASTEROID__H
#define _ASTEROID__H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "defs.h"

class Asteroid {
public:
    float x, y;
    float vy,vx;
    int width = 96, height = 96;
    bool active;
    int damage;
    AnimationData animations[1] = {
        {7, 0, 15}
    };
    bool facingLeft = true;
    int currentFrame;
    int frameTimer;
    SDL_Texture* texture;
    // Constructor với khởi tạo đầy đủ
    Asteroid(SDL_Renderer* renderer) 
        : x(SCREEN_WIDTH), y(-96.0f), vy(ASTEROID_GRAVITY), active(true), damage(10), 
          currentFrame(0), frameTimer(0) {
            vx = - (2.0f + static_cast<float>(rand()) / RAND_MAX * 6.0f);
          }

    //~Asteroid() {
    //    if (texture) {
    //        SDL_DestroyTexture(texture);
    //    }
    //}

    void update(const Player& player) {
        if (!active) return;
        if(y<GROUND_HEIGHT-height)
        {y += vy;
        x += vx;}
        if(y>=GROUND_HEIGHT-height-player.height*((int)ASTEROID_GRAVITY)/2){
       
        // Tăng frameTimer trước khi kiểm tra
        frameTimer++;
        if (frameTimer >= animations[0].frameSpeed&&currentFrame<animations[0].frameCount) {
            frameTimer = 0;
            currentFrame = (currentFrame + 1) ;
        }}
        if (y > SCREEN_HEIGHT || x < -width || x > SCREEN_WIDTH) {
            setInactive();
        }
    }

    void render(SDL_Renderer* renderer, int offsetX = 0, int offsetY = 0) {
        if (!active || !texture) return;

        const AnimationData& anim = animations[0];
        SDL_Rect clip = {
            currentFrame * width,
            anim.row * height,
            width, height
        };
        SDL_Rect destRect = { x + offsetX, y + offsetY, width, height };
        SDL_RendererFlip flipType = facingLeft ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, texture, &clip, &destRect, 0, NULL, flipType);
    }

    bool isActive() const { return active; }

    void setInactive() { active = false; }
};

#endif
