#ifndef HELP_H
#define HELP_H

#include "button.h"
#include "gamestate.h"

GameState ShowHelp(SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont(FONT, 40);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return GameState::MENU;
    }

    Button backBtn;
    backBtn.init(renderer, font, "Back", 500, SCREEN_HEIGHT-100, 200, 60);

    SDL_Event event;
    bool inHelp = true;
    
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, HELP_BACKGROUND);
    if (!bgTexture) {
        SDL_Log("Failed to load menu background: %s", IMG_GetError());
    }
    
    while (inHelp) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                TTF_CloseFont(font);
                return GameState::QUIT;
            }

            if (backBtn.handleEvent(event)) {
                TTF_CloseFont(font);
                return GameState::MENU;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr); // render ảnh nền full screen

       
        SDL_Color textColor = {255, 165, 0}; // Cam
        SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, 
            "INSTRUCTION:\n"
            "                 Control:\n"
            "- Up: Jump\n"
            "- Right: Move right\n"
            "- Left: Move left\n"
            "- Z: Dash\n"
            "- X: Punch\n"
            "- C: Kick\n"
            "Help Ori to go to new Stages by surviving waves of attack. Be careful, there are either bosses or mobs attacking with anger!",
            textColor, 600);
        SDL_Texture* helpText = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {300, 100, textSurface->w, textSurface->h};
        SDL_FreeSurface(textSurface);
        SDL_RenderCopy(renderer, helpText, nullptr, &textRect);
        SDL_DestroyTexture(helpText);

        backBtn.render(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(bgTexture);
    TTF_CloseFont(font);
    return GameState::MENU;
}

#endif // HELP_H