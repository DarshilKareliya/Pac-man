#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <string>
#include <utility>

using namespace std;

enum class Direction { UP, DOWN, LEFT, RIGHT, NONE, QUIT };
enum class MenuChoice { START, INSTRUCTIONS, LEADERBOARD, EXIT };

enum class GhostMode { CHASE, SCATTER, FRIGHTENED };

struct Position {
    int row;
    int col;
};

struct Player {
    Position position;
    Direction direction;
    int lives;
    int score;
    int coinsCollected;
    int ghostsEaten;
    int levelsCompleted;
};

struct Ghost {
    Position position;
    Direction direction;
    Position home;
    int personality; // 0=direct chaser, 1=ambusher, 2=roamer
};

struct Maze {
    vector<string> cells; // # wall, . coin, o power pellet, F bonus fruit
};

struct GameState {
    Player player;
    vector<Ghost> ghosts;
    Maze maze;
    bool gameOver;
    bool won;
    bool paused;
    bool soundEnabled;
    int level;
    int difficulty; // 1=Easy, 2=Medium, 3=Hard
    int powerTicks;
    int fruitTicks;
    int fruitRow;
    int fruitCol;
    int fruitValue;
    int ghostTicks;
    int lastFruitCoinCount;
};

// menu.cpp
MenuChoice showMenu(string& playerName, bool& soundEnabled, int& difficulty);
void showInstructions();

// maze.cpp
Maze createMaze(int level);
void drawMaze(const GameState& game);
bool isWall(const Maze& maze, Position position);
int countCoins(const Maze& maze);

// player.cpp
Direction readPlayerInput();
void movePlayer(GameState& game, Direction direction);

// ghost.cpp
void updateGhosts(GameState& game);

// game.cpp
void runGame(const string& playerName, bool soundEnabled, int difficulty);
void playGameSound(int soundType, bool enabled);

// records.cpp
void saveScore(const string& name, const GameState& game);
void showLeaderboard();
void showStatistics(const string& name, const GameState& game);

#endif

