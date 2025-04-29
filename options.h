#ifndef OPTION_H
#define OPTION_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "button.h"
#include "gamestate.h"
#include "defs.h"

GameState ShowOptions(SDL_Renderer* renderer) {
    SDL_Event event;
    TTF_Font* font = TTF_OpenFont(FONT, 32);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return GameState::QUIT;
    }

  
    int buttonWidth = 200;
    int buttonHeight = 60;
    int buttonX = 100; 
    int difficultyY = 100;
    Button difficultyBtn;
    difficultyBtn.init(renderer, font, "Difficulty", buttonX, difficultyY, buttonWidth, buttonHeight);

   
    bool showDifficultyOptions = false;
    Button easyBtn, mediumBtn, hardBtn;
    easyBtn.init(renderer, font, "Easy", 400, difficultyY, buttonWidth, buttonHeight);
    mediumBtn.init(renderer, font, "Medium", 620, difficultyY, buttonWidth, buttonHeight);
    hardBtn.init(renderer, font, "Hard", 840, difficultyY, buttonWidth, buttonHeight);

   
    int colorY = difficultyY + 80; //duoi difficulty
    Button colorBtn;
    colorBtn.init(renderer, font, "Color", buttonX, colorY, buttonWidth, buttonHeight);

   
    bool showColorOptions = false;
    Button blueBtn, pinkBtn;
    blueBtn.init(renderer, font, "Blue", 620, colorY, buttonWidth, buttonHeight);
    pinkBtn.init(renderer, font, "Pink", 400, colorY, buttonWidth, buttonHeight);

    
    int volumeY = colorY + 80; //duoi color
    Button volumeBtn;
    volumeBtn.init(renderer, font, "Volume", buttonX, volumeY, buttonWidth, buttonHeight);


    bool showVolumeSlider = false;
    SDL_Rect volumeBar = {400, volumeY + 20, 300, 20}; 
    SDL_Rect volumeFill = {400, volumeY + 20, volume * 3, 20};

   
    int backY = SCREEN_HEIGHT - 120;
    Button backBtn, saveBtn;
    backBtn.init(renderer, font, "Back", SCREEN_WIDTH / 2 - 260, backY, buttonWidth, buttonHeight);
    saveBtn.init(renderer, font, "Save", SCREEN_WIDTH / 2 + 60, backY, buttonWidth, buttonHeight);

    bool dragging = false;
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, HELP_BACKGROUND);
    if (!bgTexture) {
        SDL_Log("Failed to load menu background: %s", IMG_GetError());
        TTF_CloseFont(font);
        return GameState::MENU;
    }

    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) goto exit_options;

           
            if (difficultyBtn.handleEvent(event)) {
                showDifficultyOptions = !showDifficultyOptions;
            }

            
            if (colorBtn.handleEvent(event)) {
                showColorOptions = !showColorOptions;
            }

           
            if (volumeBtn.handleEvent(event)) {
                showVolumeSlider = !showVolumeSlider;
            }

            if (backBtn.handleEvent(event)) { TTF_CloseFont(font); return GameState::MENU; }
            if (saveBtn.handleEvent(event)) { saveConfig("data.txt"); }

            if (showDifficultyOptions) {
                if (easyBtn.handleEvent(event)) { SDL_Log("Easy selected"); currentDifficulty=Difficulty::Easy;  }
                if (mediumBtn.handleEvent(event)) { SDL_Log("Medium selected"); currentDifficulty=Difficulty::Medium; }
                if (hardBtn.handleEvent(event)) { SDL_Log("Hard selected"); currentDifficulty=Difficulty::Hard; }
            }

            if (showColorOptions) {
                if (blueBtn.handleEvent(event)) { SDL_Log("Blue selected"); gameColor=Color::Blue;}
                if (pinkBtn.handleEvent(event)) { SDL_Log("Pink selected"); gameColor=Color::Pink; }
            }

            if (showVolumeSlider) {
                
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.x >= volumeBar.x && event.button.x <= volumeBar.x + volumeBar.w) {
                    dragging = true;
                }
                if (event.type == SDL_MOUSEMOTION && dragging) {
                    int mouseX = event.motion.x;
                    if (mouseX >= volumeBar.x && mouseX <= volumeBar.x + volumeBar.w) {
                        volume = (mouseX - volumeBar.x) / 3;
                        volumeFill.w = volume * 3;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONUP) {
                    dragging = false;
                }
            }
        }

        
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

        
        difficultyBtn.render(renderer);
        colorBtn.render(renderer);
        volumeBtn.render(renderer);
        
        
        if (currentDifficulty == Difficulty::Easy) {
            easyBtn.textColor = easyBtn.selectedTextColor;
            mediumBtn.textColor = mediumBtn.normalTextColor;
            hardBtn.textColor = hardBtn.normalTextColor;
        } else if (currentDifficulty == Difficulty::Medium) {
            easyBtn.textColor = easyBtn.normalTextColor;
            mediumBtn.textColor = mediumBtn.selectedTextColor;
            hardBtn.textColor = hardBtn.normalTextColor;
        } else if (currentDifficulty == Difficulty::Hard) {
            easyBtn.textColor = easyBtn.normalTextColor;
            mediumBtn.textColor = mediumBtn.normalTextColor;
            hardBtn.textColor = hardBtn.selectedTextColor;
        }

        
        if (gameColor == Color::Blue) {
            blueBtn.textColor = blueBtn.selectedTextColor;
            pinkBtn.textColor = pinkBtn.normalTextColor;
        } else if (gameColor == Color::Pink) {
            blueBtn.textColor = blueBtn.normalTextColor;
            pinkBtn.textColor = pinkBtn.selectedTextColor;
        }

        
        easyBtn.updateTexture(renderer);
        mediumBtn.updateTexture(renderer);
        hardBtn.updateTexture(renderer);
        blueBtn.updateTexture(renderer);
        pinkBtn.updateTexture(renderer);
        

        if (showDifficultyOptions) {
            easyBtn.render(renderer);
            mediumBtn.render(renderer);
            hardBtn.render(renderer);
        }

        
        if (showColorOptions) {
            blueBtn.render(renderer);
            pinkBtn.render(renderer);
        }

       
        if (showVolumeSlider) {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderFillRect(renderer, &volumeBar);
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
            SDL_RenderFillRect(renderer, &volumeFill);
        }

       
        backBtn.render(renderer);
        saveBtn.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
        cerr<<volume<<endl;
    }

exit_options:
    if (bgTexture) SDL_DestroyTexture(bgTexture);
    difficultyBtn.destroy();
    colorBtn.destroy();
    volumeBtn.destroy();
    backBtn.destroy();
    saveBtn.destroy();
    easyBtn.destroy();
    mediumBtn.destroy();
    hardBtn.destroy();
    blueBtn.destroy();
    pinkBtn.destroy();
    TTF_CloseFont(font);
    return GameState::MENU;
}

#endif // OPTION_H