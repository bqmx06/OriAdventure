#ifndef GAMESTATE_H
#define GAMESTATE_H

enum class GameState {
    MENU,
    GAMEPLAY,
    HELP,
    OPTIONS,
    QUIT
};
enum class Difficulty {
    Easy,
    Medium,
    Hard
};
enum class Color {
    Blue,
    Pink
};

#endif // GAMESTATE_H