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
    AnimationData animations[1] = {
        {7, 0, 15}
    };
    bool facingLeft = true;
    int currentFrame;
    int frameTimer;
    SDL_Texture* texture;
    
    Asteroid(SDL_Renderer* renderer) 
        : x(SCREEN_WIDTH), y(-96.0f), vy(ASTEROID_GRAVITY), active(true), 
          currentFrame(0), frameTimer(0) {
            vx = - (2.0f + static_cast<float>(rand()) / RAND_MAX * 6.0f);
          }



    void update(Player& player) {
        int explosionTriggerY = player.y + player.height - height - player.height * ((int)ASTEROID_GRAVITY) / 2;

        bool closeToPlayerX = x >= player.x - width && x <= player.x + player.width + width;

        if (!active) return;
        if(y<GROUND_HEIGHT-height)
        {y += vy;
        x += vx;}
        if((y>=GROUND_HEIGHT-height-player.height*((int)ASTEROID_GRAVITY)/2)||(closeToPlayerX&&y >= explosionTriggerY)){
        frameTimer++;
        if (frameTimer >= animations[0].frameSpeed&&currentFrame<animations[0].frameCount) {
            frameTimer = 0;
            currentFrame++;
        }}
        if (y >= GROUND_HEIGHT-height) {
            setInactive();
            currentFrame=0;
            player.isAttacked=false;
        }
    }
    void handle(Player& player) {
        int ax = x;
        int ay = y;
        int aw = width;
        int ah = height;
    
        int px = player.x; 
        int py = player.y; 
        int pw = player.width;
        int ph = player.height;
    
        bool isCollision =
            ax + aw > px &&
            ax < px + pw &&
            ay + ah > py &&
            ay < py + ph;
            if (isCollision) {
                if (currentFrame == 3) {
                    player.isAttacked = true;
                    player.damageReceive=5;
                }
            } 
    }
    
        

        void render(SDL_Renderer* renderer) {
            if (!active || !texture) return;
        
            const AnimationData& anim = animations[0];
            SDL_Rect clip = {
                currentFrame * width, 
                anim.row * height,
                width, height
            };
          
            SDL_Rect destRect = { (int)x, (int)y, width, height }; 
            SDL_RendererFlip flipType = facingLeft ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            SDL_RenderCopyEx(renderer, texture, &clip, &destRect, 0, NULL, flipType);
        
      
        }

    void setInactive() { 
        active = false;
        x=(rand()) %SCREEN_WIDTH;
        y=-height;
        vx = -2.0f + static_cast<float>(rand()) / RAND_MAX * 4.0f;
    }
};

#endif
