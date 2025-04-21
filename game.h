#ifndef GAME_H_
#define GAME_H_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "defs.h"       
#include "gamestate.h"  
#include "player.h"     
#include "monster.h"
#include "background.h"
#include "asteroid.h"
#include <vector>
using namespace std;

inline GameState RunGame(SDL_Renderer* renderer) {
    srand(time(NULL));
    bool quit = false;
    SDL_Event event;

    
    TTF_Font* font = TTF_OpenFont(FONT, 24);
    
    Background background(renderer);
    background.texture=IMG_LoadTexture(renderer, BG_IMAGE_PATH);
    background.generatePlatforms(renderer); // Sinh nền tảng ngẫu nhiên cho màn đầu tiên
    const char* selectedTexture=gameColor==Color::Pink?PLAYER_SPRITE_PATH:PLAYER_SPRITE_PATH_BLUE;
    Player player(renderer);
    player.texture = IMG_LoadTexture(renderer, selectedTexture);
    
    Monster monster(renderer);
    monster.texture = IMG_LoadTexture(renderer, MONSTER_SPRITE_PATH);


    SDL_Texture* asteroidTexture=IMG_LoadTexture(renderer, ASTEROID_PATH);
    Uint32 lastAsteroidBatchTime = SDL_GetTicks();
    vector<Asteroid> asteroids;

    for (int i = 0; i < ASTEROID; ++i) {
        Asteroid asteroid(renderer);
        asteroid.texture = asteroidTexture;
        asteroid.active = false;
        asteroids.push_back(asteroid);
    }



    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) quit = true;
            player.handleEvent(event);
        }
        
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastAsteroidBatchTime >= 4000) { 
            int startX = rand() % (SCREEN_WIDTH - 96 * 3);
            for (auto& asteroid : asteroids) {
                if (!asteroid.active) {
                    asteroid.active = true;
                }
            }
            lastAsteroidBatchTime = currentTime;
        }
        for (auto& asteroid : asteroids) {
            asteroid.update(player);
        }
        player.update();
        
        monster.update();
        monster.handle(player);
        
        for (auto& asteroid : asteroids) {
            asteroid.handle(player);
        }
        background.handle(player);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);                    
        
        background.render(renderer);        // Vẽ nền
        background.renderPlatforms(renderer); // Vẽ các nền tảng
        player.render(renderer); 
        monster.render(renderer);
        for (auto& asteroid1 : asteroids) {
            asteroid1.render(renderer);
            //cerr<<asteroid.x<<" "<<asteroid.y<<endl;
        }

        SDL_RenderPresent(renderer); 
        SDL_Delay(10); 
        //cerr<<player.isAttacked<<endl;
    }
    TTF_CloseFont(font);
    SDL_DestroyTexture(asteroidTexture);

    return GameState::MENU; 
}

#endif // GAME_H_