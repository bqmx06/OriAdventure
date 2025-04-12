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
enum class PlayerState {
    IDLE,
    RUNNING,
    JUMPING,
    PUNCHING,
    KICKING,
    DASHING
};
class PlayerStateManager {
    public:
        
        PlayerStateManager() : currentState(PlayerState::IDLE) {}
    
        
        void setState(PlayerState newState){ currentState = newState;}
    
        
        PlayerState getState() const { return currentState; }
    
    private:
        PlayerState currentState; };

#endif // GAMESTATE_H