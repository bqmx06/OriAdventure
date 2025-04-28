#ifndef DEFS_H
#define DEFS_H
#include"gamestate.h"
#include <string>
#define WINDOW_TITLE "Ori Adventure"
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define FONT "assets/font/Coiny-Regular.ttf"
#define MENU_BACKGROUND "assets/background/menu.jpg"
#define HELP_BACKGROUND "assets/background/menu.png"

#define BG_IMAGE_PATH "assets/background/background.png"
#define BG_WIDTH 2400
#define BG_SPEED 0.8f

#define ASTEROID_GRAVITY 5.0f
#define ASTEROID 10
#define ASTEROID_PATH "assets/asteroid/asteroid.png"


#define PLAYER_SPRITE_PATH "assets/player/PinkOriSprite.png"
#define PLAYER_SPRITE_PATH_BLUE "assets/player/BlueOriSprite1.png"
#define MONSTER_SPRITE_PATH "assets/monster/DemonKingSprite.png"
#define GROUND_HEIGHT SCREEN_HEIGHT-85
#define SPEED 5
#define MONSTER_SPEED 2
#define JUMPFORCE -15

#define AVATAR "assets/player/avatar.png"
#define FRAME "assets/player/avatarframe.png"

extern Difficulty currentDifficulty;
extern Color gameColor;
extern int volume;
extern int highscore;
void readConfig(const std::string& path);
void saveConfig(const std::string& path);


#endif // DEFS_H
