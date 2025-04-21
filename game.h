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

int Stage=0;

int MonsterCount=0;

bool ClearStage(){
    return MonsterCount==0;
}

void SpawnMonsters(SDL_Renderer* renderer, vector<Monster*>& monsters) {
    monsters.clear();
    MonsterCount = Stage * 2;
    cerr << "Spawn MonsterCount = " << MonsterCount << endl;

    for (int i = 0; i < MonsterCount; ++i) {
        Monster* m = new Monster(renderer);
        m->texture = IMG_LoadTexture(renderer, MONSTER_SPRITE_PATH);
        m->x = rand() % (SCREEN_WIDTH - 100) + 50; // vị trí random trên màn
        monsters.push_back(m);
    }
}
void ReRender(SDL_Renderer* renderer, Player& player, Background& background,vector<Monster*>& monsters) {
    // 1. Fade to black
    Stage++;
    //MonsterCount=Stage*2;
    Uint32 startTime = SDL_GetTicks();
    Uint32 duration = 1000; // 1 giây

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    while (SDL_GetTicks() - startTime < duration) {
        float progress = (float)(SDL_GetTicks() - startTime) / duration;
        int alpha = (int)(progress * 255);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60fps
    }

    // 2. Reset lại vị trí player
    player.x = SCREEN_WIDTH / 2;
    player.y = GROUND_HEIGHT - player.height;
    player.vx = 0;
    player.vy = 0;
    player.isJumping = false;
    player.isAttacked = false;
    player.facingLeft= false;
    player.setState(PlayerState::IDLE);

    // 3. Reset background nếu có scroll offset hoặc trạng thái
    background.reset(renderer); // Giả sử bạn có hàm reset() cho background
    //3.1 monster
    SpawnMonsters(renderer, monsters);
    // 4. Fade from black
    startTime = SDL_GetTicks();
    while (SDL_GetTicks() - startTime < duration) {
        float progress = (float)(SDL_GetTicks() - startTime) / duration;
        int alpha = 255 - (int)(progress * 255);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}



inline GameState RunGame(SDL_Renderer* renderer) {
    srand(time(NULL));
    bool quit = false;
    SDL_Event event;

    
    TTF_Font* font = TTF_OpenFont(FONT, 24);
    
    Background background(renderer);
    background.texture=IMG_LoadTexture(renderer, BG_IMAGE_PATH);
    background.generatePlatforms(renderer);
    const char* selectedTexture=gameColor==Color::Pink?PLAYER_SPRITE_PATH:PLAYER_SPRITE_PATH_BLUE;
    Player player(renderer);
    player.texture = IMG_LoadTexture(renderer, selectedTexture);
    
    vector<Monster*> monsters;
    SpawnMonsters(renderer, monsters);


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
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE &&player.x<SCREEN_WIDTH) {Stage=0; MonsterCount=0; quit = true;}
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
        
        for (auto it = monsters.begin(); it != monsters.end(); ) {
            Monster* m = *it;
            m->update();
            m->handle(player);

            if (m->isDestroyed) {
                delete m;
                it = monsters.erase(it);
                MonsterCount--;
            } else {
                ++it;
            }
        }
        for (auto& asteroid : asteroids) {
            asteroid.handle(player);
        }
        background.handle(player);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);                    
        
        background.render(renderer);        // Vẽ nền
        background.renderPlatforms(renderer); // Vẽ các nền tảng
        player.render(renderer); 
        for (auto& m : monsters) {
            m->render(renderer);
        }
        for (auto& asteroid1 : asteroids) {
            asteroid1.render(renderer);
            //cerr<<asteroid.x<<" "<<asteroid.y<<endl;
        }

        SDL_RenderPresent(renderer); 
        SDL_Delay(10); 
        if(ClearStage()&&player.x>SCREEN_WIDTH)
        ReRender(renderer,player,background,monsters);
    }
    TTF_CloseFont(font);
    SDL_DestroyTexture(asteroidTexture);

    return GameState::MENU; 
}

#endif // GAME_H_