#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum class Direction {
    UP, DOWN, LEFT, RIGHT, NONE, QUIT
};

enum class MenuChoice {
    START, INSTRUCTIONS, LEADERBOARD, EXIT
};

struct Position {
    int row;
    int col;
};

struct Player {
    Position position;
    Direction direction;
    int lives;
    int score;
};

struct Ghost {
    Position position;
    Direction direction;
};

struct Maze {
    vector<string> cells;
};

struct GameState {
    Player player;
    vector<Ghost> ghosts;
    Maze maze;
    bool gameOver;
    bool won;
};

// menu.cpp
MenuChoice showMenu(string& playerName);
void showInstructions();

// maze.cpp
Maze createMaze();
void drawMaze(const GameState& game);
bool isWall(const Maze& maze, Position position);
int countCoins(const Maze& maze);

// player.cpp
Direction readPlayerInput();
void movePlayer(GameState& game, Direction direction);

// ghost.cpp
void updateGhosts(GameState& game);

// game.cpp
void runGame(const string& playerName);

// records.cpp
void saveScore(const string& name, int score);
void showLeaderboard();

#endif
