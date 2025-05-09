#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include"defs.h"
#include"game.h"
#include"options.h"
#include"button.h"
#include"menu.h" 
#include"help.h"
#include<string>
#include<vector>

Difficulty currentDifficulty;
Color gameColor;
int volume;
int highscore;

using namespace std;
int main(int argc, char* argv[]) {
    readConfig("data.txt");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    //SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    

    //SDL_ttf
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Failed: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    //SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio Failed: " << Mix_GetError() << std::endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    Mix_Music* menuMusic = Mix_LoadMUS("assets/music/menu.wav");
    Mix_Music* gameMusic = Mix_LoadMUS("assets/music/game.mp3");
    Mix_Music* currentMusic = nullptr;


    



    if (!menuMusic || !gameMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL_CreateWindow Failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    //renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    GameState currentState = GameState::MENU;
    bool running = true;

    while (running) {
        
        readConfig("data.txt");
        volume=volume*128/100;
        Mix_VolumeMusic(volume);
        Mix_Music* desiredMusic = nullptr;

        if (currentState == GameState::GAMEPLAY) {
            desiredMusic = gameMusic;
        }
        else if (currentState == GameState::MENU || 
                 currentState == GameState::HELP || 
                 currentState == GameState::OPTIONS) {
            desiredMusic = menuMusic;
        }
        if (desiredMusic != currentMusic) {
            if (Mix_PlayingMusic()) {
                Mix_HaltMusic();  // dừng bài cũ
            }
            Mix_PlayMusic(desiredMusic, -1); // -1 = lặp vô hạn
            currentMusic = desiredMusic;
        }
        switch (currentState) {
            case GameState::MENU:
                currentState = ShowMenu(renderer);
                break;
            case GameState::GAMEPLAY:
                currentState = RunGame(renderer);
                break;
            case GameState::HELP:
                currentState = ShowHelp(renderer);
                break;
            case GameState::OPTIONS:
                currentState = ShowOptions(renderer);
                readConfig("data.txt");
                Mix_VolumeMusic(volume);
                break;
            case GameState::QUIT:
                running = false;
                break;
        }
    }

    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    Mix_FreeMusic(menuMusic);
    Mix_FreeMusic(gameMusic);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}