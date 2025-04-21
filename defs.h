#ifndef DEFS_H
#define DEFS_H
#include"gamestate.h"
#define WINDOW_TITLE "Ori Adventure"
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define FONT "assets/font/Coiny-Regular.ttf"
#define MENU_BACKGROUND "assets/background/menu.jpg"
#define HELP_BACKGROUND "assets/background/menu.png"

#define BG_IMAGE_PATH "assets/background/background.png"
#define BG_WIDTH 2400
#define BG_SPEED 0.5f

#define ASTEROID_GRAVITY 5.0f
#define ASTEROID 10
#define ASTEROID_PATH "assets/asteroid/asteroid.png"

#define LEFTZONE 300
#define RIGHTZONE 900


#define PLAYER_SPRITE_PATH "assets/player/OriSprite.png"
#define PLAYER_SPRITE_PATH_BLUE "assets/player/BlueOriSprite1.png"
#define MONSTER_SPRITE_PATH "assets/monster/DemonKingSprite.png"
#define GROUND_HEIGHT SCREEN_HEIGHT-85
#define SPEED 3
#define MONSTER_SPEED 0
#define JUMPFORCE -15


extern Difficulty currentDifficulty;
extern Color gameColor;
int volume=50;

#endif // DEFS_H
