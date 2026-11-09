#include "game.h"
#include <conio.h>

Direction readPlayerInput() {
    if (!_kbhit()) {
        return Direction::NONE;
    }

    int key = _getch();

    // Arrow keys produce a prefix followed by a second key code.
    if (key == 0 || key == 224) {
        key = _getch();

        switch (key) {
            case 72: return Direction::UP;
            case 80: return Direction::DOWN;
            case 75: return Direction::LEFT;
            case 77: return Direction::RIGHT;
        }
    }

    switch (key) {
        case 'w':
        case 'W': return Direction::UP;

        case 's':
        case 'S': return Direction::DOWN;

        case 'a':
        case 'A': return Direction::LEFT;

        case 'd':
        case 'D': return Direction::RIGHT;

        case 'q':
        case 'Q': return Direction::QUIT;

        default: return Direction::NONE;
    }
}

void movePlayer(GameState& game, Direction direction) {
    // A new direction replaces the current direction.
    if (direction != Direction::NONE) {
        game.player.direction = direction;
    }

    if (game.player.direction == Direction::NONE) {
        return;
    }

    Position next = game.player.position;

    switch (game.player.direction) {
        case Direction::UP:    next.row--; break;
        case Direction::DOWN:  next.row++; break;
        case Direction::LEFT:  next.col--; break;
        case Direction::RIGHT: next.col++; break;
        default: return;
    }

    // Stop moving when the next cell is a wall.
    if (isWall(game.maze, next)) {
        game.player.direction = Direction::NONE;
        return;
    }

    game.player.position = next;

    // Collect a coin.
    if (game.maze.cells[next.row][next.col] == '.') {
        game.player.score += 10;
        game.maze.cells[next.row][next.col] = ' ';
    }
}
