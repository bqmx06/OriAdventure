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
#include "laser.h"
#include <vector>
using namespace std;

int Stage=0;

int MonsterCount=0;





bool ClearStage(){
    return MonsterCount==0;
}

void SpawnMonsters(SDL_Renderer* renderer, vector<Monster*>& monsters) {
    monsters.clear();
    float coefficient;
    if(currentDifficulty==Difficulty::Easy) coefficient=1.2;
    else if(currentDifficulty==Difficulty::Medium) coefficient=1.8;
    else coefficient=2.0;
    MonsterCount = (Stage==0)?0:(int)((float)Stage * coefficient)+rand()%3;
    //cerr << "Spawn MonsterCount = " << MonsterCount << endl;

    for (int i = 0; i < MonsterCount; ++i) {
        Monster* m = nullptr;
        int dice1=rand()%6+1;
        int dice2=rand()%6+1;
        if (dice1+dice2 <9) {
            m = new Monster(renderer);
            m->texture = IMG_LoadTexture(renderer, MONSTER_SPRITE_PATH);
        } 
        else if (dice1+dice2>9) {
            m = new EliteMonster1(renderer);
            m->texture = IMG_LoadTexture(renderer, ELITEMONSTER1_SPRITE_PATH);
        } 
        else {
            m = new EliteMonster2(renderer);
            m->texture = IMG_LoadTexture(renderer, ELITEMONSTER2_SPRITE_PATH);
        }
        m->x = (rand() % 3600)-1800+600; 
        monsters.push_back(m);
    }
}
void ReRender(SDL_Renderer* renderer, Player& player, Background& background,vector<Monster*>& monsters, const vector<string>& bgPaths) {
    //Fade to black
    Stage++;
    if(Stage>highscore)
    highscore=Stage;
    saveConfig("data.txt");
    Uint32 startTime = SDL_GetTicks();
    Uint32 duration = 1000; 

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    while (SDL_GetTicks() - startTime < duration) {
        float progress = (float)(SDL_GetTicks() - startTime) / duration;
        int alpha = (int)(progress * 255);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }

    //Reset lại vị trí player
    player.x = SCREEN_WIDTH / 2;
    player.y = GROUND_HEIGHT - player.height;
    player.vx = 0;
    player.vy = 0;
    player.isJumping = false;
    player.isAttacked = false;
    player.facingLeft= false;
    player.setState(PlayerState::IDLE);
    if (currentDifficulty == Difficulty::Hard) player.hp+=10;
    else if(currentDifficulty == Difficulty::Easy) player.hp+=30;
    else player.hp+=20; 
    
    if(player.hp>=100)
    player.hp=100;


    int idx = rand() % int(bgPaths.size());
    const string& path = bgPaths[idx];

    if (background.texture) {
        SDL_DestroyTexture(background.texture);
    }

    //Load background mới
    background.texture = IMG_LoadTexture(renderer, path.c_str());
    if (!background.texture) {
        cerr << "Failed to load BG: " << path 
                << " error: " << IMG_GetError() << "\n";
    }
    background.reset(renderer); 
    //monster
    SpawnMonsters(renderer, monsters);
    //Fade from black
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
    
    std::vector<std::string> bgPaths={
        BACKGROUND1,
        BACKGROUND2,
        BACKGROUND3,
        BACKGROUND4,
        BACKGROUND5,
        BACKGROUND6,
        BACKGROUND7,
    };
    std::vector<SDL_Texture*> bgTextures;
    Background background(renderer);
    background.texture=IMG_LoadTexture(renderer,BACKGROUND1);
    background.generatePlatforms(renderer);
    const char* selectedTexture=gameColor==Color::Pink?PLAYER_SPRITE_PATH:PLAYER_SPRITE_PATH_BLUE;
    Player player(renderer);
    player.resetRegen();

    player.texture = IMG_LoadTexture(renderer, selectedTexture);
    player.avatarTexture = IMG_LoadTexture(renderer, "assets/player/avatar.png");
    player.frameTexture  = IMG_LoadTexture(renderer, "assets/player/avatarframe.png");
    player.fireball.texture= IMG_LoadTexture(renderer, "assets/player/fireball.png");
    player.laserTexture= IMG_LoadTexture(renderer, "assets/player/laser.png");
    
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
            if (event.type == SDL_KEYUP
                && event.key.keysym.sym == SDLK_c
                && player.currentState == PlayerState::ULTIMATE)
            {
                player.energy=0;
                // toạ độ tâm quả cầu
                float fx = player.fireball.x + player.fireball.width/2;
                float fy = player.fireball.y + player.fireball.height/2;
        
                // spawn 1 laser cho mỗi monster
                for (auto* m : monsters) {
                    if(m->facingLeft!=player.facingLeft){
                    float mx = m->x + m->width/2;
                    float my = m->y + m->height;
                    player.lasers.emplace_back(
                        fx, fy, mx, my,
                        player.laserTexture,
                        /*lifetime=*/2000
                    );}

                }
                for (auto* m : monsters) {
                    if(m->facingLeft!=player.facingLeft){
                        m->hp=0;
                    
                    }
                    
                    
                }

                


            }
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
        
        background.render(renderer);        
        background.renderPlatforms(renderer); 
        player.render(renderer); 
        
        for (auto& m : monsters) {
            m->render(renderer);
        }
        for (auto& asteroid1 : asteroids) {
            asteroid1.render(renderer);
        }



        // --- Vẽ Stage ---
        std::string stageText = "Stage: " + std::to_string(Stage);
        SDL_Color textColor = {255, 255, 255, 255}; // Trắng
        SDL_Surface* stageSurface = TTF_RenderText_Solid(font, stageText.c_str(), textColor);
        SDL_Texture* stageTexture = SDL_CreateTextureFromSurface(renderer, stageSurface);
        int stageW, stageH;
        SDL_QueryTexture(stageTexture, NULL, NULL, &stageW, &stageH);
        SDL_Rect stageRect = { SCREEN_WIDTH / 2 - stageW / 2, 10, stageW, stageH };
        SDL_RenderCopy(renderer, stageTexture, NULL, &stageRect);
        SDL_FreeSurface(stageSurface);
        SDL_DestroyTexture(stageTexture);

        // --- Vẽ Highscore ---
        std::string hsText = "Highscore: " + std::to_string(highscore);
        SDL_Surface* hsSurface = TTF_RenderText_Solid(font, hsText.c_str(), textColor);
        SDL_Texture* hsTexture = SDL_CreateTextureFromSurface(renderer, hsSurface);
        int hsW, hsH;
        SDL_QueryTexture(hsTexture, NULL, NULL, &hsW, &hsH);
        SDL_Rect hsRect = { SCREEN_WIDTH - hsW - 20, 10, hsW, hsH };
        SDL_RenderCopy(renderer, hsTexture, NULL, &hsRect);
        SDL_FreeSurface(hsSurface);
        SDL_DestroyTexture(hsTexture);


        static Uint32 deathTime = 0;

        if (player.currentState == PlayerState::DEAD&&!player.fakeDead) {

            if (deathTime == 0) {
                deathTime = SDL_GetTicks();
            }

            if (SDL_GetTicks() - deathTime > 3500) { 
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
                SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_RenderFillRect(renderer, &overlay);


                SDL_Color red = {255, 0, 0, 255};
                SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "GAME OVER", red);
                SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
                int goW, goH;
                SDL_QueryTexture(gameOverTexture, NULL, NULL, &goW, &goH);
                SDL_Rect goRect = {SCREEN_WIDTH / 2 - goW / 2, SCREEN_HEIGHT / 2 - goH / 2 - 30, goW, goH};
                SDL_RenderCopy(renderer, gameOverTexture, NULL, &goRect);
                SDL_FreeSurface(gameOverSurface);
                SDL_DestroyTexture(gameOverTexture);


                SDL_Color white = {255, 255, 255, 255};
                SDL_Surface* infoSurface = TTF_RenderText_Solid(font, "Press R to Retry or ESC to Quit", white);
                SDL_Texture* infoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
                int infoW, infoH;
                SDL_QueryTexture(infoTexture, NULL, NULL, &infoW, &infoH);
                SDL_Rect infoRect = {SCREEN_WIDTH / 2 - infoW / 2, SCREEN_HEIGHT / 2 + 20, infoW, infoH};
                SDL_RenderCopy(renderer, infoTexture, NULL, &infoRect);
                SDL_FreeSurface(infoSurface);
                SDL_DestroyTexture(infoTexture);

                SDL_RenderPresent(renderer);


                bool wait = true;
                while (wait) {
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            wait = false;
                            quit = true;
                        } else if (event.type == SDL_KEYDOWN) {
                            if (event.key.keysym.sym == SDLK_r) {
                                deathTime = 0; 
                                Stage = 0;
                                MonsterCount = 0;
                                return GameState::GAMEPLAY;
                            } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                                Stage= 0;
                                wait = false;
                                quit = true;
                            }
                        }
                    }
                    SDL_Delay(10);
                }
            }
        }
        else {
            deathTime = 0;
        }
                

        player.renderBars(renderer); 
        player.renderAvatar(renderer);

        SDL_RenderPresent(renderer); 
        SDL_Delay(10); 
        if(ClearStage()&&player.x>SCREEN_WIDTH)
        ReRender(renderer,player,background,monsters,bgPaths);
    }
    TTF_CloseFont(font);
    SDL_DestroyTexture(asteroidTexture);

    return GameState::MENU; 
}

#endif // GAME_H_