#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

#include "defs.h"
#include "math.h"
using namespace std;
struct Platform {
    int x, y, width, height;
    SDL_Texture* texture;

    Platform(int x, int y, int width, int height, SDL_Texture* texture)
        : x(x), y(y), width(width), height(height), texture(texture) {}
	void handle(Player& player) {
		if (checkCollision(player)) {
			player.y = y - player.height+15; 
			player.vy = 0; 
			player.isJumping = false;
			if(player.currentState==PlayerState::JUMPING){
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
                if (currentKeyStates[SDL_SCANCODE_RIGHT]||currentKeyStates[SDL_SCANCODE_LEFT]) {
                    player.currentState=PlayerState::RUNNING;
                } else {
                    player.currentState=PlayerState::IDLE;
                }
			}
		}
	}


	bool checkCollision(const Player& player) {
		bool verticalCheck = player.vy >= 0 &&  
			player.y + player.height <= y + 15 && 
			player.y + player.height >= y - 10;   
		bool horizontalCheck = (player.x + player.width > x && player.x < x + width);
		return verticalCheck && horizontalCheck;
	}
};
class Background {
	public:
	int x,y;
	int width,height;
	//texture
	SDL_Texture* texture;
	vector<Platform> platforms;
	SDL_Texture* platformTexture1 = nullptr;
	SDL_Texture* platformTexture2 = nullptr;
	Background(SDL_Renderer* renderer):x(-(BG_WIDTH-SCREEN_WIDTH)/2),y(0),width(BG_WIDTH),height(SCREEN_HEIGHT){
	}
	~Background() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
		for(auto& platform :platforms)
		SDL_DestroyTexture(platform.texture);
    }
	void handle(Player& player) {
		if (player.vx != 0) {
			int deltaX = player.vx/BG_SPEED;
			x -= deltaX; 
			if(x>0) x=0;
			else if(x<-1200) x=-1200;
			else
			for (auto& platform : platforms) {
				platform.x -= deltaX;
			}
		}
		for (auto& platform : platforms) {
            platform.handle(player); 
        }
	}
	void render(SDL_Renderer* renderer) {
        if (!texture) return;
        SDL_Rect destRect = { x, y, width, height };
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }

	
	void generatePlatforms(SDL_Renderer* renderer) {
		platforms.clear(); 
		int numPlatforms = 10 + rand() % 15; 
		for (int i = 0; i < numPlatforms; i++) {
			int platformWidth = 100 + rand() % 200; 
			int platformHeight = 20; 
			int platformX = -1800 + rand() % 3600; 
			int platformY = 200 + rand() % (SCREEN_HEIGHT - 400); 
			
			SDL_Texture* platformTexture = IMG_LoadTexture(renderer, "platform.png");
			if (platformTexture) {
				platforms.emplace_back(platformX, platformY, platformWidth, platformHeight, platformTexture);
			}
		}
	}

	void renderPlatforms(SDL_Renderer* renderer) {
		for (const auto& platform : platforms) {
			SDL_Rect destRect = { platform.x, platform.y, platform.width, platform.height };
			SDL_RenderCopy(renderer, platform.texture, nullptr, &destRect);
		}
	}

	void reset(SDL_Renderer* renderer) {
		
		x = -(BG_WIDTH - SCREEN_WIDTH) / 2;
		y = 0;
	
	
		for (auto& platform : platforms) {
			if (platform.texture) {
				SDL_DestroyTexture(platform.texture);
				platform.texture = nullptr;
			}
		}
		platforms.clear();
	
		
		generatePlatforms(renderer);
	}
	



};





#endif