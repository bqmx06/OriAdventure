#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "defs.h" 
#include "math.h"
using namespace std;





class Player {
public:
    //rect
    int x, y;
    int width=90, height=111;

    // Movement velocity
    float vx, vy;
    //bool
    bool facingLeft=false;
    bool isJumping=false;
    Uint32 dashDuration=200;
    Uint32 dashStartTime=0;
    // Spritesheet texture
    SDL_Texture* texture;

    // Animation-related members
    AnimationData animations[8] = {
        {8, 0, 10},  // IDLE: 8 frames, row 0
        {7, 1, 10},  // RUNNING: 7 frames, row 1
        {10, 2, 6}, // JUMPING: 10 frames, row 2
        {5, 3, 5},  // PUNCHING: 5 frames, row 3
        {5, 4, 5},  // KICKING: 5 frames, row 4
        {3, 5, 5},   // DASHING: 3 frames, row 5
        {2, 6, 10},  //HURT
        {8, 7, 10}  //DEAD
    };
    int currentFrame;
    int frameTimer;
    PlayerState currentState; 

    // Constructor
    Player(SDL_Renderer* renderer)
        : x(SCREEN_WIDTH / 2),
          y(GROUND_HEIGHT), 
          width(90), 
          height(111), 
          vx(0), vy(0),
          texture(nullptr),
          currentFrame(0),
          frameTimer(0), 
          currentState(PlayerState::IDLE)
    {}

    // Destructor
    ~Player() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    // Update player's position and animation
    void update() {
        const float gravity = 0.5f;
        // Apply gravity
        vy += gravity;
        x += (int)(vx);
        y += (int)(vy);
        // frame
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
            currentFrame = (currentFrame + 1) % animations[(int)(currentState)].frameCount;
        }
    }


    void render(SDL_Renderer* renderer) {
        if (!texture) return;
        const AnimationData& anim = animations[(int)(currentState)];
        cerr<< (int)currentState<<endl;
        
        SDL_Rect clip = {
            currentFrame * width, 
            anim.row * height,   
            width, height
        };
        SDL_Rect destRect = { x, y, width, height };
        SDL_RendererFlip flipType = facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        // Render the current frame
        SDL_RenderCopyEx(renderer, texture, &clip, &destRect,0,NULL,flipType);
    }

    // Handle user input
    void handleEvent(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    facingLeft=true;
                    vx = -SPEED;
                    if(!isJumping)
                    setState(PlayerState::RUNNING);
                    break;
                case SDLK_RIGHT:
                    facingLeft=false;
                    vx = SPEED;
                    if(!isJumping)
                    setState(PlayerState::RUNNING);
                    break;
                case SDLK_UP:
                    if (!isJumping&&currentState!=PlayerState::DASHING) {
                        vy = JUMPFORCE;
                        setState(PlayerState::JUMPING);
                        isJumping=true;
                    }
                    break;
                case SDLK_x:
                    if(currentState!=PlayerState::RUNNING)
                    setState(PlayerState::PUNCHING);
                    break;
                case SDLK_c:
                    if(currentState!=PlayerState::RUNNING)
                    setState(PlayerState::KICKING);
                    break;
                case SDLK_z:
                    if(currentState==PlayerState::IDLE||currentState==PlayerState::RUNNING)
                    {setState(PlayerState::DASHING);
                    vx=facingLeft?-3*SPEED:3*SPEED;
                    dashStartTime=SDL_GetTicks();}
                    break;
                case SDLK_v:
                    setState(PlayerState::HURT);
                    break;
                case SDLK_b:
                    setState(PlayerState::DEAD);
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
                case SDLK_v:
                case SDLK_x:
                case SDLK_c:{
                    if(!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT]&&!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT]){
                    if(isJumping)
                    setState(PlayerState::JUMPING);
                    else
                    setState(PlayerState::IDLE);}
                    break;}                    
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
        }
    }
};

#endif // PLAYER_H