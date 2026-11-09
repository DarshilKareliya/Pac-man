#include "game.h"
#include <windows.h>

// Create the maze. '#' = wall, '.' = coin, ' ' = empty path.
Maze createMaze() {
    Maze maze;

    maze.cells = {
        "#####################",
        "#.........#.........#",
        "#.###.###.#.###.###.#",
        "#.#...#...#...#...#.#",
        "#.#.###.#####.###.#.#",
        "#...................#",
        "###.#.###.###.#.###.#",
        "#...#.....#...#.....#",
        "#.#####.#.#.#.#####.#",
        "#.......#...#.......#",
        "#.###.###.#.###.###.#",
        "#.#...#...#...#...#.#",
        "#.###.#.#####.#.###.#",
        "#.........#.........#",
        "#####################"
    };

    return maze;
}

bool isWall(const Maze& maze, Position position) {
    if (position.row < 0 ||
        position.row >= static_cast<int>(maze.cells.size())) {
        return true;
    }

    if (position.col < 0 ||
        position.col >= static_cast<int>(maze.cells[position.row].size())) {
        return true;
    }

    return maze.cells[position.row][position.col] == '#';
}

int countCoins(const Maze& maze) {
    int count = 0;

    for (const string& row : maze.cells) {
        for (char cell : row) {
            if (cell == '.') {
                count++;
            }
        }
    }

    return count;
}

void drawMaze(const GameState& game) {
    COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition
    );

    vector<string> screen = game.maze.cells;

    for (const Ghost& ghost : game.ghosts) {
        screen[ghost.position.row][ghost.position.col] = 'G';
    }

    screen[game.player.position.row][game.player.position.col] = 'C';

    cout << "========== PAC-MAN ==========\n\n";

    for (const string& row : screen) {
        cout << row << '\n';
    }

    cout << "\nPlayer: " << game.player.score
         << "    Lives: " << game.player.lives
         << "    Coins left: " << countCoins(game.maze)
         << "\nControls: W A S D / Arrow Keys | Q: Quit\n";

    cout.flush();
}
