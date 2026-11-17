#include "game.h"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <conio.h>
#else
#include <cstdio>
#endif

static bool pauseRequested = false;

bool consumePauseRequest() {
    bool value = pauseRequested;
    pauseRequested = false;
    return value;
}

Direction readPlayerInput() {
#ifdef _WIN32
    if (!_kbhit()) return Direction::NONE;
    int key = _getch();
    if (key == 0 || key == 224) {
        key = _getch();
        switch (key) {
            case 72: return Direction::UP;
            case 80: return Direction::DOWN;
            case 75: return Direction::LEFT;
            case 77: return Direction::RIGHT;
        }
    }
#else
    if (!std::cin.rdbuf()->in_avail()) return Direction::NONE;
    int key = std::getchar();
#endif
    switch (key) {
        case 'w': case 'W': return Direction::UP;
        case 's': case 'S': return Direction::DOWN;
        case 'a': case 'A': return Direction::LEFT;
        case 'd': case 'D': return Direction::RIGHT;
        case 'q': case 'Q': return Direction::QUIT;
        case 'p': case 'P':
            pauseRequested = true;
            return Direction::NONE;
        default: return Direction::NONE;
    }
}

static void collectCell(GameState& game, Position p) {
    char& cell = game.maze.cells[p.row][p.col];
    if (cell == '.') {
        game.player.score += 10;
        ++game.player.coinsCollected;
        cell = ' ';
        playGameSound(0, game.soundEnabled);
    } else if (cell == 'o') {
        game.player.score += 50;
        game.powerTicks = 65;
        cell = ' ';
        playGameSound(1, game.soundEnabled);
    }
}

void movePlayer(GameState& game, Direction direction) {
    if (direction != Direction::NONE) game.player.direction = direction;
    if (game.player.direction == Direction::NONE) return;

    Position next = game.player.position;
    switch (game.player.direction) {
        case Direction::UP: --next.row; break;
        case Direction::DOWN: ++next.row; break;
        case Direction::LEFT: --next.col; break;
        case Direction::RIGHT: ++next.col; break;
        default: return;
    }

    if (isWall(game.maze, next)) {
        game.player.direction = Direction::NONE;
        return;
    }
    game.player.position = next;
    collectCell(game, next);
}
