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
#define PLAYER_SPRITE_PATH "assets/player/OriSprite.png"
#define GROUND_HEIGHT SCREEN_HEIGHT-85
#define SPEED 5
#define JUMPFORCE -15


extern Difficulty currentDifficulty;
extern Color gameColor;
int volume=50;

#endif // DEFS_H
