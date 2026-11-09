#include "game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

void updateGhosts(GameState& game) {
    static bool randomSeeded = false;

    if (!randomSeeded) {
        srand(static_cast<unsigned int>(time(0)));
        randomSeeded = true;
    }

    const Direction directions[] = {
        Direction::UP,
        Direction::DOWN,
        Direction::LEFT,
        Direction::RIGHT
    };

    for (Ghost& ghost : game.ghosts) {
        vector<Direction> possibleDirections;

        // Find directions that are not blocked by walls.
        for (Direction direction : directions) {
            Position next = ghost.position;

            switch (direction) {
                case Direction::UP:    next.row--; break;
                case Direction::DOWN:  next.row++; break;
                case Direction::LEFT:  next.col--; break;
                case Direction::RIGHT: next.col++; break;
                default: break;
            }

            if (!isWall(game.maze, next)) {
                possibleDirections.push_back(direction);
            }
        }

        if (possibleDirections.empty()) {
            continue;
        }

        // Most of the time, move towards the player.
        if (rand() % 100 < 65) {
            int bestDistance = 100000;
            vector<Direction> bestDirections;

            for (Direction direction : possibleDirections) {
                Position next = ghost.position;

                switch (direction) {
                    case Direction::UP:    next.row--; break;
                    case Direction::DOWN:  next.row++; break;
                    case Direction::LEFT:  next.col--; break;
                    case Direction::RIGHT: next.col++; break;
                    default: break;
                }

                int distance =
                    abs(next.row - game.player.position.row) +
                    abs(next.col - game.player.position.col);

                if (distance < bestDistance) {
                    bestDistance = distance;
                    bestDirections.clear();
                    bestDirections.push_back(direction);
                } else if (distance == bestDistance) {
                    bestDirections.push_back(direction);
                }
            }

            ghost.direction =
                bestDirections[rand() % bestDirections.size()];
        } else {
            // Sometimes choose a random valid direction.
            ghost.direction =
                possibleDirections[rand() % possibleDirections.size()];
        }

        // Move the ghost one cell.
        switch (ghost.direction) {
            case Direction::UP:    ghost.position.row--; break;
            case Direction::DOWN:  ghost.position.row++; break;
            case Direction::LEFT:  ghost.position.col--; break;
            case Direction::RIGHT: ghost.position.col++; break;
            default: break;
        }
    }
}
