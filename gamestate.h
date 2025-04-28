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
    DASHING,
    HURT,
    DEAD,
    CHARGING,
    ULTIMATE
};
class PlayerStateManager {
    public:
        
        PlayerStateManager() : currentState(PlayerState::IDLE) {}
    
        
        void setState(PlayerState newState){ currentState = newState;}
    
        
        PlayerState getState() const { return currentState; }
    
    private:
        PlayerState currentState; };
enum class MonsterState {
    WALKING,
    ATTACKING,
    HURT,
    DEAD
};
class MonsterStateManager {
    public:
        
        MonsterStateManager() : currentState(MonsterState::WALKING) {}
    
        
        void setState(MonsterState newState){ currentState = newState;}
    
        
        MonsterState getState() const { return currentState; }
    
    private:
        MonsterState currentState; };
class AnimationData {
    public:
        int frameCount; // Number of frames in the animation
        int row;  
        int frameSpeed;      // Row number in the spritesheet for the animation

        AnimationData(int frameCount = 1, int row = 0,int frameSpeed = 10)
            : frameCount(frameCount), row(row), frameSpeed(frameSpeed) {}
};
        
#endif // GAMESTATE_H