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
    SDL_Rect rect;
    

    // Movement velocity
    float vx;
    bool facingLeft = false;

    Uint32 attackCooldown = 1000; // Cooldown giữa các lần tấn công
    Uint32 lastAttackTime = 0;    // Thời gian lần tấn công cuối

    // Spritesheet texture
    SDL_Texture* texture;

    // Animation-related members
    AnimationData animations[3] = {
		{12,0,5},
		{13,1,5},
		{5,2,5}
		
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
        rect={x,y,width,height};
    }

    // Destructor
    ~Monster() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    // Update monster's position and animation
    void update() {
        if (currentState != MonsterState::ATTACKING) {
            x += (int)(vx);
        }      
        // Animation frames
        frameTimer++;
        if (frameTimer >= animations[(int)(currentState)].frameSpeed) {
            frameTimer = 0;
            currentFrame = (currentFrame + 1) % animations[(int)(currentState)].frameCount;
        }
        if (currentState == MonsterState::ATTACKING &&
            currentFrame == animations[(int)(currentState)].frameCount - 1) {
            setState(MonsterState::WALKING);
        }
        rect = {x, y, width, height};
    }

    void render(SDL_Renderer* renderer) {
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
        player.isAttacked = false;
        int playerCenter = player.x + player.width / 2;
        int monsterCenter = x + width / 2;
        int distance = abs(monsterCenter - playerCenter);
        int attackRange=player.width;

        if (distance > attackRange) {
     
            vx = (playerCenter > monsterCenter) ? abs(MONSTER_SPEED) : -abs(MONSTER_SPEED);
    
            facingLeft = vx < 0;
        } else {
            
            vx = 0;
            attack(player);
           
}
  
    }
    // Attack logic (ví dụ quái vật tấn công trong khoảng thời gian nhất định)
    void attack(Player& player) {
        
    
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastAttackTime > attackCooldown) {
            lastAttackTime = currentTime;
            setState(MonsterState::ATTACKING);
            //cerr << "[MONSTER ATTACK]\n";
            //cerr << "Monster - x: " << x << ", y: " << y << ", w: " << width << ", h: " << height << endl;
            //cerr << "Player  - x: " << player.x << ", y: " << player.y << ", w: " << player.width << ", h: " << player.height << endl;
    
        }
        if(currentState==MonsterState::ATTACKING&&currentFrame==8)
        player.isAttacked=true;
        //cerr<<"Monster isAttacked: "<<player.isAttacked<<endl;
        
    }
    
};

#endif // MONSTER_H