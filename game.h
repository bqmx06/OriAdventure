#ifndef GAME_H_
#define GAME_H_

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include "defs.h"       
#include "gamestate.h"  
#include "player.h"     
using namespace std;

inline GameState RunGame(SDL_Renderer* renderer) {
    bool quit = false;
    SDL_Event event;

   
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, BG_IMAGE_PATH);
    if (!bgTexture) {
        std::cerr << "Failed to load background texture: " << IMG_GetError() << std::endl;
        return GameState::MENU;
    }

    
    TTF_Font* font = TTF_OpenFont(FONT, 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyTexture(bgTexture);
        return GameState::MENU;
    }

    Player player(renderer);
    player.texture = IMG_LoadTexture(renderer, PLAYER_SPRITE_PATH);
  
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                quit = true; 
            }

         
            player.handleEvent(event);
        }

     
        player.update();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);                    
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr); 

        player.render(renderer); 

        SDL_RenderPresent(renderer); 
        SDL_Delay(10); 
    }


    SDL_DestroyTexture(bgTexture);
    TTF_CloseFont(font);

    return GameState::MENU; 
}

#endif // GAME_H_