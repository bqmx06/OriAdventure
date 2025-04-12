#ifndef MENU_H
#define MENU_H

#include "button.h"
#include "gamestate.h"

GameState ShowMenu(SDL_Renderer* renderer) {
    SDL_Event event;
    TTF_Font* font = TTF_OpenFont(FONT, 32);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return GameState::QUIT;
    }

    
    Button playBtn, helpBtn, optionsBtn, quitBtn;
    playBtn.init(renderer, font, "Play",     500, 200, 200, 60);
    helpBtn.init(renderer, font, "Help",     500, 280, 200, 60);
    optionsBtn.init(renderer, font, "Options", 500, 360, 200, 60);
    quitBtn.init(renderer, font, "Quit",     500, 440, 200, 60);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, MENU_BACKGROUND);
    if (!bgTexture) {
        SDL_Log("Failed to load menu background: %s", IMG_GetError());
    }
    
   
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) goto exit_menu;

            if (playBtn.handleEvent(event))   { TTF_CloseFont(font); return GameState::GAMEPLAY; }
            if (helpBtn.handleEvent(event))   { TTF_CloseFont(font); return GameState::HELP; }
            if (optionsBtn.handleEvent(event)){ TTF_CloseFont(font); return GameState::OPTIONS; }
            if (quitBtn.handleEvent(event))   { TTF_CloseFont(font); return GameState::QUIT; }
        }

        
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr); // render ảnh nền full screen

        playBtn.render(renderer);
        helpBtn.render(renderer);
        optionsBtn.render(renderer);
        quitBtn.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

exit_menu:
    SDL_DestroyTexture(bgTexture);
    playBtn.destroy();
    helpBtn.destroy();
    optionsBtn.destroy();
    quitBtn.destroy();
    TTF_CloseFont(font);
    return GameState::MENU;
}
#endif // MENU_H
