#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "defs.h" 
#include "laser.h"
#include "math.h"
#include <algorithm>
#include <vector>
using namespace std;


class Player {
public:
    //rect
    int width=90, height=111;
    int x, y;
    // Movement velocity
    float vx, vy;
    //bool
    bool facingLeft=false;
    bool isJumping=false;
    bool isAttacked=false;
    bool fakeDead=false;
        // freeze effect
    bool freeze = false;
    Uint32 freezeStartTime = 0;
    Uint32 freezeDuration = 2000; 

    // flame effect
    bool flame = false;
    Uint32 flameStartTime = 0;
    Uint32 flameDuration = 4000; 
    Uint32 lastBurnTime = 0;  
    Uint32 burnInterval = 500; 
    int burnDamage = 5;
    //dash
    Uint32 dashDuration=200;
    Uint32 dashStartTime=0;
    //hurt
    bool isInvincible = false; 
    Uint32 invincibleStartTime = 0; 
    Uint32 invincibleDuration = 900; 

    //stats
    int hp=100;
    int energy=100;
    int strength=100;
    int damage=2;
    int damageReceive=10;
    //attack
    int comboIndex = 0;              
    Uint32 lastAttackTime = 0;       
    Uint32 comboResetDelay = 1500;     
    // Regeneration
    Uint32 lastRegenTime = 0;          
    Uint32 regenInterval = 1000;       
    int regenHP;                    
    int regenStrength;             
    int regenEnergy;              
    // Spritesheet texture
    SDL_Texture* texture;
    SDL_Texture* avatarTexture = nullptr;
    SDL_Texture* frameTexture = nullptr;
    SDL_Texture* laserTexture =nullptr;
    
    //laser
    Fireball fireball;
    std::vector<Laser> lasers;


    // Animation-related members
    AnimationData animations[10] = {
        {8, 0, 10},  // IDLE: 8 frames, row 0
        {7, 1, 5},  // RUNNING: 7 frames, row 1
        {10, 2, 6}, // JUMPING: 10 frames, row 2
        {5, 3, 5},  // PUNCHING: 5 frames, row 3
        {5, 4, 5},  // KICKING: 5 frames, row 4
        {3, 5, 5},   // DASHING: 3 frames, row 5
        {2, 6, 10},  //HURT
        {8, 7, 10},  //DEAD
        {3, 8, 10},
        {7, 9, 15}
    };
    int currentFrame;
    int frameTimer;
    PlayerState currentState; 

    // Constructor
    Player(SDL_Renderer* renderer)
        : x(SCREEN_WIDTH / 2),
          y(GROUND_HEIGHT-height), 
          width(90), 
          height(111), 
          vx(0), vy(0),
          texture(nullptr),
          currentFrame(0),
          frameTimer(0), 
          currentState(PlayerState::IDLE)
    {   
    }

    // Destructor
    ~Player() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
        if (avatarTexture) SDL_DestroyTexture(avatarTexture);
        if (frameTexture) SDL_DestroyTexture(frameTexture);
        if (laserTexture) SDL_DestroyTexture(laserTexture);

    }

    
    void updateHealth(){
        if((!isInvincible)&&(currentState==PlayerState::IDLE||currentState==PlayerState::RUNNING||currentState==PlayerState::JUMPING))
        {   if (isAttacked)
            {   
                hp-=damageReceive;
                setState(PlayerState::HURT);
            }
        } 
    }
    void update() {
        if(fakeDead)
        {regenHP=-10;}
        else if(hp>9&&currentState!=PlayerState::CHARGING)
        {if(currentDifficulty==Difficulty::Easy||currentDifficulty==Difficulty::Medium) regenHP=1;
        else regenHP=0;}
        
        Uint32 now = SDL_GetTicks();
        //effect
        if (freeze) {
            if (now - freezeStartTime > freezeDuration) {
                freeze = false;
            } else {
                vx = 0; 
            }
        }
        
        // flame effect
        if (flame) {
            if (now - flameStartTime > flameDuration) {
                flame = false;
            } else if (now - lastBurnTime > burnInterval) {
                hp -= burnDamage;
                lastBurnTime = now;
            }
        }
        // Regeneration
        if (now - lastRegenTime >= regenInterval) {
            lastRegenTime = now;
            hp = min(hp + regenHP, 100);
            strength = min(strength + regenStrength, 100);
            energy = min(energy + regenEnergy, 100);
        }
        //fireball
        fireball.x = x + width / 2 - fireball.width / 2;
        fireball.y = y - 300;

        if (currentState==PlayerState::CHARGING||currentState==PlayerState::ULTIMATE) {
            fireball.update();
        } else {
            fireball.currentFrame = 0;
            fireball.frameTimer = 0;
        }
        for (auto& l : lasers) 
        l.update();
        lasers.erase(
            std::remove_if(lasers.begin(), lasers.end(),
                           [](const Laser& l){ return !l.isActive; }),
            lasers.end()
          );
        if (currentState == PlayerState::ULTIMATE && lasers.empty()) {
            setState(PlayerState::IDLE);
            fireball.isCharged=false;
        }
        const float gravity = 0.5f;
        // Apply gravity
        vy += gravity;
        x += (int)(vx);
        y += (int)(vy);

        
        //frame
        frameTimer ++;
        // Handle ground collision
        if (y + height > GROUND_HEIGHT) {
            y = GROUND_HEIGHT - height;
            vy = 0;
            if (isJumping) {
                isJumping = false;
                const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
                if (currentKeyStates[SDL_SCANCODE_RIGHT]||currentKeyStates[SDL_SCANCODE_LEFT]) {
                    setState(PlayerState::RUNNING);
                } else {
                    setState(PlayerState::IDLE);
                }
            }
        }
        if (isInvincible) {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - invincibleStartTime > invincibleDuration) {
                isInvincible = false;
            }
        }
        // attacked
    
        updateHealth();

        if(hp<=0)
        {hp=0;
        vx=0;
        regenHP=0;
        fakeDead=false;
        setState(PlayerState::DEAD);}
        if(strength<=0)
        strength=0;
        if(energy<=0)
        energy=0;

        
        // Handle dash
        if (currentState == PlayerState::DASHING) {
            Uint32 currentDashTime = SDL_GetTicks();
            if (currentDashTime - dashStartTime > dashDuration) {
                const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
                if (currentKeyStates[SDL_SCANCODE_RIGHT]||currentKeyStates[SDL_SCANCODE_LEFT]) {
                    setState(PlayerState::RUNNING);
                    vx=facingLeft?-SPEED:SPEED;
                } else {
                    setState(PlayerState::IDLE);
                    vx = 0; 
                }
                
            }
        }
        //handle hurt dead
        if (currentState == PlayerState::DEAD) {
            if (frameTimer >= animations[(int)(currentState)].frameSpeed) {
                frameTimer = 0;
                if (currentFrame < animations[(int)(currentState)].frameCount - 1) {
                    currentFrame++;
                }
            }
        }
        if(currentState == PlayerState::JUMPING){
            float v0=fabs(JUMPFORCE);
            float maxHeight = (v0 * v0) / (2 * gravity);  
            float totalFrame = v0/gravity+sqrt((2 * maxHeight) / gravity);  
            animations[(int)(currentState)].frameSpeed=totalFrame / animations[(int)(currentState)].frameCount;
        }
        if (currentState==PlayerState::DASHING){
            if (frameTimer >= animations[(int)(currentState)].frameSpeed) {
                frameTimer=0;
                if(currentFrame<animations[(int)(currentState)].frameCount-1) currentFrame++;
            }
        }
        if (frameTimer >= animations[(int)(currentState)].frameSpeed) {
            frameTimer = 0;
            if (currentState == PlayerState::HURT) {
                if (currentFrame < animations[(int)(currentState)].frameCount - 1) {
                    currentFrame++;
                } else {
                    isAttacked = false; 
                    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
                    if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_LEFT]) {
                        setState(PlayerState::RUNNING);
                    } else {
                        setState(PlayerState::IDLE);
                    }
                }
            }
            else if (currentState == PlayerState::CHARGING || currentState == PlayerState::ULTIMATE){
                if (currentFrame < animations[(int)(currentState)].frameCount - 1) 
                    currentFrame++;
            }
            else if (currentState == PlayerState::PUNCHING || currentState == PlayerState::KICKING) {
                if (currentFrame < animations[(int)(currentState)].frameCount - 1) {
                    currentFrame++;
                } else {            
                    comboIndex++;
                    if (comboIndex > 2) comboIndex = 0; // combo kết thúc
            
                    if (isJumping)
                        setState(PlayerState::JUMPING);
                    else
                        setState(PlayerState::IDLE);
                }
            }
            else {
                currentFrame = (currentFrame + 1) % animations[(int)(currentState)].frameCount;
            }
        }
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
        SDL_RendererFlip flipType = facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

        SDL_RenderCopyEx(renderer, texture, &clip, &destRect,0,NULL,flipType);
        for (auto& l : lasers) 
            l.render(renderer);
        if (currentState==PlayerState::CHARGING||currentState==PlayerState::ULTIMATE) {
            fireball.render(renderer);
        }
        

    }
    void renderAvatar(SDL_Renderer* renderer) {
        if (!avatarTexture || !frameTexture) return;
    
        SDL_Rect avatarRect = { 0, -10, 128, 128 };
    
        SDL_RenderCopy(renderer, avatarTexture, nullptr, &avatarRect);   
        SDL_RenderCopy(renderer, frameTexture, nullptr, &avatarRect);    
    }
        
    
    void renderBars(SDL_Renderer* renderer) {
        int barWidth = 100;
        int barHeight = 10;
        int offsetY = 15;
    
    
        int baseX = 105;
        int baseY = 30;
    

        SDL_Rect hpBorder = { baseX - 1, baseY - 1, barWidth + 2, barHeight + 2 };
        SDL_Rect hpBack = { baseX, baseY, barWidth, barHeight };
        SDL_Rect hpFront = { baseX, baseY, barWidth * hp / 100, barHeight };
    
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &hpBorder);
        SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
        SDL_RenderFillRect(renderer, &hpBack);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &hpFront);
    
 
        int sy = baseY + offsetY;
        SDL_Rect strengthBorder = { baseX - 1, sy - 1, barWidth + 2, barHeight + 2 };
        SDL_Rect strengthBack = { baseX, sy, barWidth, barHeight };
        SDL_Rect strengthFront = { baseX, sy, barWidth * strength / 100, barHeight };
    
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &strengthBorder);
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &strengthBack);
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        SDL_RenderFillRect(renderer, &strengthFront);
    

        int ey = baseY + 2 * offsetY;
        SDL_Rect energyBorder = { baseX - 1, ey - 1, barWidth + 2, barHeight + 2 };
        SDL_Rect energyBack = { baseX, ey, barWidth, barHeight };
        SDL_Rect energyFront = { baseX, ey, barWidth * energy / 100, barHeight };
    
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &energyBorder);
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &energyBack);
        SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255);
        SDL_RenderFillRect(renderer, &energyFront);
    }
    
    
    
    // Handle user input
    void handleEvent(const SDL_Event& event) {
        if ((currentState==PlayerState::DEAD&&!fakeDead)||currentState==PlayerState::ULTIMATE)
        return;
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&!freeze) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    if(fakeDead)
                    fakeDead=false;
                    facingLeft=true;
                    vx = -SPEED;
                    if(!isJumping)
                    setState(PlayerState::RUNNING);
                    break;
                case SDLK_RIGHT:
                    if(fakeDead)
                    fakeDead=false;
                    facingLeft=false;   
                    vx = SPEED;
                    if(!isJumping)
                    setState(PlayerState::RUNNING);
                    break;
                case SDLK_UP:
                    if(fakeDead)
                    fakeDead=false;
                    if (!isJumping && currentState != PlayerState::DASHING) { 
                        vy = JUMPFORCE;
                        setState(PlayerState::JUMPING);
                        isJumping=true;
                    }
                    break;
                case SDLK_x:
                    if(strength>=10)
                    if (currentState == PlayerState::IDLE || currentState == PlayerState::JUMPING) {
                        Uint32 now = SDL_GetTicks();
                        if (now - lastAttackTime > comboResetDelay) {
                            comboIndex = 0; 
                        }
                
                        lastAttackTime = now;
                        if (comboIndex == 0 || comboIndex == 1) {
                            setState(PlayerState::PUNCHING);
                            strength-=10;
                        } else if (comboIndex == 2) {
                            setState(PlayerState::KICKING);
                            strength-=20;
                        }
                    }
                    break;
                case SDLK_c:
                    if(energy==100){
                    if (currentState == PlayerState::IDLE ) {
                        setState(PlayerState::CHARGING);
                        regenHP=0;
                        regenEnergy=0;
                        regenStrength=0;
                    }}
                    break;
                case SDLK_z:
                    if(energy>=20){
                    if(currentState==PlayerState::IDLE||currentState==PlayerState::RUNNING)
                    {setState(PlayerState::DASHING);
                    energy-=20;
                    vx=facingLeft?-3*SPEED:3*SPEED;
                    dashStartTime=SDL_GetTicks();}}
                    break;
                case SDLK_b:
                    {setState(PlayerState::DEAD);
                    fakeDead=true;}
                    break;
                default:
                    break;
            }
        }
        if (event.type == SDL_KEYUP && event.key.repeat == 0) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    if(!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT]){
                    vx=0;
                    if(!isJumping)
                    setState(PlayerState::IDLE);}
                    break;
                case SDLK_RIGHT:
                    if(!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT]){
                    vx=0;
                    if(!isJumping)
                    setState(PlayerState::IDLE);}
                    break;
                case SDLK_c:
                    if (currentState == PlayerState::CHARGING) {
                        resetRegen();
                        if(fireball.isCharged)
                        setState(PlayerState::ULTIMATE);
                        else setState(PlayerState::IDLE);
                        

                    }
                    break;                 
                default:
                    break;
            }
        }
    }
    void setState(PlayerState state) {
       
        if (currentState != state) {
            currentState = state;
            currentFrame = 0;
            frameTimer = 0;
            if (state == PlayerState::HURT) {
                isAttacked = true;
                isInvincible = true; 
                invincibleStartTime = SDL_GetTicks();
            }
        }
    }
    void resetRegen(){
        if (currentDifficulty == Difficulty::Hard) {
            regenHP = 0;
            regenEnergy = 1;
            regenStrength = 10;
            regenInterval=6000;
        } else if (currentDifficulty == Difficulty::Medium) {
            regenHP = 1;
            regenEnergy = 1;
            regenStrength = 5;
            regenInterval=2000;
        } else {
            regenHP = 1;
            regenEnergy = 3;
            regenStrength = 10;
        }

    }
    
};

#endif // PLAYER_H