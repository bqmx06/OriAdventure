#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "defs.h"
#include "math.h"
using namespace std;

class Background {
	public:
	int x,y;
	int width,height;
	//texture
	SDL_Texture* texture;
	Background(SDL_Renderer* renderer):x(-(BG_WIDTH-SCREEN_WIDTH)/2),y(0),width(BG_WIDTH),height(SCREEN_HEIGHT){}
	~Background() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
    }
	void handle(const Player& player){
		if (player.vx != 0) {
			x -= (player.vx > 0 ? 1 : -1) * BG_SPEED;
		}
	}
	void render(SDL_Renderer* renderer) {
        if (!texture) return;
        SDL_Rect destRect = { x, y, width, height };
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }
};





#endif