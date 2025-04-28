#ifndef MONSTER_H
#define MONSTER_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "defs.h"
#include "math.h"
using namespace std;


class Monster {
public:
    //rect
    int width = 240, height = 160;
    int x, y;    

    // Movement velocity
    float vx;
    bool facingLeft = false;
    bool isDestroyed=false;

    Uint32 attackCooldown = 1000; // Cooldown giữa các lần tấn công
    Uint32 lastAttackTime = 0;    // Thời gian lần tấn công cuối
    //stats
    int hp=200;
    // Spritesheet texture
    SDL_Texture* texture;

    // Animation-related members
    AnimationData animations[4] = {
		{12,0,5},
		{13,1,5},
		{5,2,5},
        {14,3,15}
		
};
    int currentFrame;
    int frameTimer;
    MonsterState currentState;

    // Constructor
    Monster(SDL_Renderer* renderer)
        : x(SCREEN_WIDTH),
          y(GROUND_HEIGHT-height),
          vx(MONSTER_SPEED),
          texture(nullptr),
          currentFrame(0),
          frameTimer(0),
          currentState(MonsterState::WALKING)          
    {
    }

    // Destructor
    ~Monster() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    // Update monster's position and animation
    void update() {
        if (hp <= 0 && currentState != MonsterState::DEAD) {
            hp = 0;
            setState(MonsterState::DEAD);
        }
        
        if (currentState != MonsterState::ATTACKING&&currentState!=MonsterState::DEAD) {
            x += (int)(vx);
        }      
        // Animation frames
        frameTimer++;
        if (currentState == MonsterState::DEAD)
        {
            if(currentFrame != animations[(int)(currentState)].frameCount - 1)
            currentFrame++;
            else
            isDestroyed=true;
            return;
        }
        else
        {
        if (frameTimer >= animations[(int)(currentState)].frameSpeed) {
            frameTimer = 0;
            currentFrame = (currentFrame + 1) % animations[(int)(currentState)].frameCount;
        }}
        if (currentState == MonsterState::ATTACKING &&
            currentFrame == animations[(int)(currentState)].frameCount - 1) {
            setState(MonsterState::WALKING);
        }
    }

    void render(SDL_Renderer* renderer)  {
        if (!texture) return;
        const AnimationData& anim = animations[(int)(currentState)];
        SDL_Rect clip = {
            currentFrame * width,
            anim.row * height,
            width, height
        };
        SDL_Rect destRect = { x, y, width, height };
        SDL_RendererFlip flipType = facingLeft ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, texture, &clip, &destRect, 0, NULL, flipType);
    }

    // Set state
    void setState(MonsterState state) {
        if (currentState != state) {
            currentState = state;
            currentFrame = 0;
            frameTimer = 0;
        }
    }
 
	void handle(Player& player) {
        if (currentState == MonsterState::DEAD) return;

        int playerCenter = player.x + player.width / 2;
        int monsterCenter = x + width / 2;
        int distance = abs(monsterCenter - playerCenter);
        int attackRange=player.width;

        if (distance > attackRange) {
            setState(MonsterState::WALKING);
            //if(player.currentState==PlayerState::DASHING)
            //x+=10*((player.facingLeft)*2-1);
            x+=abs(player.vx)*((player.facingLeft)*2-1)/3;
            vx = (playerCenter > monsterCenter) ? abs(MONSTER_SPEED) : -abs(MONSTER_SPEED);
    
            facingLeft = vx < 0;
        } else {
            vx = 0;
            if(currentState!=MonsterState::DEAD){
            if((player.currentState==PlayerState::PUNCHING||player.currentState==PlayerState::KICKING)&&(player.facingLeft!=facingLeft))
            hurt();
            else
            attack(player);}
        }
  
    }
    
    // Attack logic (ví dụ quái vật tấn công trong khoảng thời gian nhất định)
    void attack(Player& player) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastAttackTime > attackCooldown) {
            lastAttackTime = currentTime;
            setState(MonsterState::ATTACKING);    
        }
        if(currentState==MonsterState::ATTACKING&&currentFrame==8){
        SDL_Rect playerRect = { player.x, player.y, player.width, player.height };
        SDL_Rect monsterRect = { x, y, width, height };

        if (SDL_HasIntersection(&playerRect, &monsterRect)) {
            player.isAttacked = true;
            player.damage = 20;
        }
        }
    }
    void hurt(){
        setState(MonsterState::HURT);
        hp-=1;
    }
};

#endif // MONSTER_H