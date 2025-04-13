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
    int x, y;
    int width = 90, height = 111;

    // Movement velocity
    float vx, vy;
    bool facingLeft = false;

    Uint32 attackCooldown = 1000; // Cooldown giữa các lần tấn công
    Uint32 lastAttackTime = 0;    // Thời gian lần tấn công cuối

    // Spritesheet texture
    SDL_Texture* texture;

    // Animation-related members
    AnimationData animations[4] = {
        {8, 0, 10},  // IDLE: 8 frames, row 0
        {7, 1, 10},  // RUNNING: 7 frames, row 1
        {5, 2, 5},   // ATTACKING: 5 frames, row 2
        {8, 3, 10}   // DEAD: 8 frames, row 3
    };
    int currentFrame;
    int frameTimer;
    MonsterState currentState;

    // Constructor
    Monster(SDL_Renderer* renderer)
        : x(SCREEN_WIDTH / 3),
          y(GROUND_HEIGHT),
          vx(0), vy(0),
          texture(nullptr),
          currentFrame(0),
          frameTimer(0),
          currentState(MonsterState::WALKING)
    {}

    // Destructor
    ~Monster() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }

    // Update monster's position and animation
    void update() {
        const float gravity = 0.5f;
        vy += gravity;
        x += (int)(vx);
        y += (int)(vy);

        // Handle ground collision
        if (y + height > GROUND_HEIGHT) {
            y = GROUND_HEIGHT - height;
            vy = 0;
        }

        // Animation frames
        frameTimer++;
        if (frameTimer >= animations[(int)(currentState)].frameSpeed) {
            frameTimer = 0;
            currentFrame = (currentFrame + 1) % animations[(int)(currentState)].frameCount;
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

    // Attack logic (ví dụ quái vật tấn công trong khoảng thời gian nhất định)
    void attack() {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastAttackTime > attackCooldown) {
            lastAttackTime = currentTime;
            setState(MonsterState::ATTACKING);
        }
    }
};

#endif // MONSTER_H