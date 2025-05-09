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
    virtual int getSpeed() const {
        return 3;
    }
    //rect
    int width = 240, height = 160;
    int x, y;    

    // Movement velocity
    float vx;
    bool facingLeft = false;
    bool isDestroyed=false;

    Uint32 attackCooldown = 1000; 
    Uint32 lastAttackTime = 0;    
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
          vx(getSpeed()),
          texture(nullptr),
          currentFrame(0),
          frameTimer(0),
          currentState(MonsterState::WALKING)          
    {
    }

    // Destructor
    virtual ~Monster() {
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
            x+=abs(player.vx)*((player.facingLeft)*2-1)/3;
            vx = (playerCenter > monsterCenter) ? abs(getSpeed()) : -abs(getSpeed());
    
            facingLeft = vx < 0;
        } else {
            vx = 0;
            if(currentState!=MonsterState::DEAD){
            if((player.currentState==PlayerState::PUNCHING||player.currentState==PlayerState::KICKING)&&(player.facingLeft!=facingLeft))
            hurt(player);
            else
            attack(player);}
        }
  
    }
    
   
    virtual void attack(Player& player) {
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
            player.damageReceive = 20;
        }
        }
    }
    void hurt(const Player& player){
        setState(MonsterState::HURT);
        if(player.comboIndex==2)
        hp-=2;
        else hp-=1;
    }
};
class EliteMonster1 : public Monster {
    public:
        EliteMonster1(SDL_Renderer* renderer) : Monster(renderer) {
            texture = IMG_LoadTexture(renderer, "elite1.png"); 
            hp=150;
            attackCooldown=5000;
        }
        int getSpeed() const override {
            return 1;  
        }
        void attack(Player& player) override {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastAttackTime > attackCooldown) {
                lastAttackTime = currentTime;
                setState(MonsterState::ATTACKING);    
            }
            if(currentState == MonsterState::ATTACKING && currentFrame == 8) { 
                SDL_Rect playerRect = { player.x, player.y, player.width, player.height };
                SDL_Rect monsterRect = { x, y, width, height };
    
                if (SDL_HasIntersection(&playerRect, &monsterRect)) {
                    player.isAttacked = true;
                    player.freeze=true;
                    player.freezeStartTime=SDL_GetTicks();
                    player.damageReceive = 15;
                }
            }
        }
    };
class EliteMonster2 : public Monster {
    public:
        EliteMonster2(SDL_Renderer* renderer) : Monster(renderer) {
            texture = IMG_LoadTexture(renderer, "elite2.png"); 
            hp=200;
            attackCooldown=3000;
        }
        int getSpeed() const override {
            return 2;  
        }
        void attack(Player& player) override {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastAttackTime > attackCooldown) {
                lastAttackTime = currentTime;
                setState(MonsterState::ATTACKING);    
            }
            if(currentState == MonsterState::ATTACKING && currentFrame == 8) { 
                SDL_Rect playerRect = { player.x, player.y, player.width, player.height };
                SDL_Rect monsterRect = { x, y, width, height };
    
                if (SDL_HasIntersection(&playerRect, &monsterRect)) {
                    player.isAttacked = true;
                    player.flame=true;
                    player.flameStartTime = SDL_GetTicks();
                    player.damageReceive = 25;
                }
            }
        }
    };
#endif // MONSTER_H