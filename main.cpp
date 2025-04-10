#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#include"defs.h"
#include"graphics.h"
#include"game.h"
#include"options.h"
#include"button.h"
#include"menu.h"

GameState ShowHelp(SDL_Renderer* renderer); // forward declaration


int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Khởi tạo SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Failed: " << TTF_GetError() << std::endl;
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

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Quản lý trạng thái game
    GameState currentState = GameState::MENU;
    bool running = true;

    while (running) {
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
    SDL_Quit();

    return 0;
}