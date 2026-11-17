#include "game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

static Position step(Position p, Direction d) {
    switch (d) {
        case Direction::UP: --p.row; break;
        case Direction::DOWN: ++p.row; break;
        case Direction::LEFT: --p.col; break;
        case Direction::RIGHT: ++p.col; break;
        default: break;
    }
    return p;
}

void updateGhosts(GameState& game) {
    static bool seeded = false;
    if (!seeded) { srand(static_cast<unsigned int>(time(0))); seeded = true; }

    const Direction dirs[] = {Direction::UP, Direction::DOWN,
                              Direction::LEFT, Direction::RIGHT};
    ++game.ghostTicks;
    GhostMode mode = game.powerTicks > 0 ? GhostMode::FRIGHTENED :
        ((game.ghostTicks % 28 < 8) ? GhostMode::SCATTER : GhostMode::CHASE);

    for (size_t i = 0; i < game.ghosts.size(); ++i) {
        Ghost& ghost = game.ghosts[i];
        vector<Direction> possible;
        for (Direction d : dirs) {
            Position next = step(ghost.position, d);
            if (!isWall(game.maze, next)) possible.push_back(d);
        }
        if (possible.empty()) continue;

        Direction chosen = possible[rand() % possible.size()];
        int best = mode == GhostMode::FRIGHTENED ? -1 : 100000;

        for (Direction d : possible) {
            Position next = step(ghost.position, d);
            Position target = game.player.position;

            // Ghost personalities: direct chase, target slightly ahead,
            // and more random patrol movement.
            if (ghost.personality == 1) {
                if (game.player.direction == Direction::UP) target.row -= 2;
                if (game.player.direction == Direction::DOWN) target.row += 2;
                if (game.player.direction == Direction::LEFT) target.col -= 2;
                if (game.player.direction == Direction::RIGHT) target.col += 2;
            }

            int distance = abs(next.row - target.row) + abs(next.col - target.col);
            if (ghost.personality == 2 && mode == GhostMode::CHASE && rand() % 3 == 0)
                distance = rand() % 30;

            if (mode == GhostMode::SCATTER) {
                distance = abs(next.row - ghost.home.row) + abs(next.col - ghost.home.col);
            }

            if (mode == GhostMode::FRIGHTENED) distance = -distance;
            if ((mode == GhostMode::FRIGHTENED && distance < best) ||
                (mode != GhostMode::FRIGHTENED && distance < best)) {
                best = distance;
                chosen = d;
            }
        }
        ghost.direction = chosen;
        ghost.position = step(ghost.position, chosen);
    }
}

