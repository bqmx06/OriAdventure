#ifndef BUTTON_H
#define BUTTON_H

#include<string>
using namespace std;

struct Button {
    SDL_Rect rect;
    SDL_Color normalColor = { 100, 100, 100, 255 };
    SDL_Color hoverColor = { 150, 150, 150, 255 };
    SDL_Color currentColor = normalColor;
    string label;
    TTF_Font* font = nullptr;
    SDL_Texture* textTexture = nullptr;
    SDL_Rect textRect;

    void init(SDL_Renderer* renderer, TTF_Font* font_, const string& text, int x, int y, int w, int h) {
        rect = { x, y, w, h };
        font = font_;
        label = text;
        updateTexture(renderer);
    }

    void updateTexture(SDL_Renderer* renderer) {
        if (textTexture) SDL_DestroyTexture(textTexture);

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, label.c_str(), {255, 255, 255});
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = { rect.x + (rect.w - textSurface->w) / 2, rect.y + (rect.h - textSurface->h) / 2, textSurface->w, textSurface->h };
        SDL_FreeSurface(textSurface);
    }

    bool handleEvent(const SDL_Event& e) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        bool inside = (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);

        if (inside) {
            currentColor = hoverColor;
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                return true;
            }
        } else {
            currentColor = normalColor;
        }

        return false;
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        SDL_RenderFillRect(renderer, &rect);
        if (textTexture) {
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        }
    }

    void destroy() {
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }
    }
};

#endif // BUTTON_H
