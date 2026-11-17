
#include "game.h"
#include <windows.h>

// Create the maze.
// # = wall, . = coin, space = empty path.
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


// Check whether a position is a wall.
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


// Count remaining coins.
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


// Check whether a neighbouring cell is a wall.
bool hasWall(const vector<string>& cells, int row, int col) {
    if (row < 0 || row >= static_cast<int>(cells.size())) {
        return false;
    }

    if (col < 0 || col >= static_cast<int>(cells[row].size())) {
        return false;
    }

    return cells[row][col] == '#';
}


// Draw connected wall characters.
string wallCharacter(const vector<string>& cells, int row, int col) {
    bool up    = hasWall(cells, row - 1, col);
    bool down  = hasWall(cells, row + 1, col);
    bool left  = hasWall(cells, row, col - 1);
    bool right = hasWall(cells, row, col + 1);

    if (up && down && left && right) return "┼";
    if (up && down && left) return "┤";
    if (up && down && right) return "├";
    if (up && left && right) return "┴";
    if (down && left && right) return "┬";
    if (up && right) return "└";
    if (up && left) return "┘";
    if (down && right) return "┌";
    if (down && left) return "┐";
    if (left || right) return "─";
    if (up || down) return "│";

    return "┼";
}


void drawMaze(const GameState& game) {
    SetConsoleOutputCP(CP_UTF8);

    COORD cursorPosition = {0, 0};
    SetConsoleCursorPosition(
        GetStdHandle(STD_OUTPUT_HANDLE),
        cursorPosition
    );

    vector<string> screen = game.maze.cells;

    for (const Ghost& ghost : game.ghosts) {
        screen[ghost.position.row][ghost.position.col] = 'G';
    }

    screen[game.player.position.row][game.player.position.col] = 'C';

    int width = static_cast<int>(game.maze.cells[0].size());

    // Decorative ASCII frame around the existing maze.
    cout << "   +" << string(width, '-') << "+\n";

    for (int row = 0; row < static_cast<int>(screen.size()); row++) {
        cout << "   |";
        for (int col = 0; col < static_cast<int>(screen[row].size()); col++) {
            char cell = screen[row][col];

            if (cell == '#') {
                cout << wallCharacter(game.maze.cells, row, col);
            }
            else if (cell == '.') {
                cout << "\033[33m.\033[0m";
            }
            else if (cell == 'C') {
                cout << "\033[93mC\033[0m";
            }
            else if (cell == 'G') {
                cout << "\033[91mG\033[0m";
            }
            else {
                cout << " ";
            }
        }

        cout << "|\n";
    }

    // Bottom border
    cout << "   +" << string(width, '-') << "+\n";

    cout << "\n  Score: " << game.player.score
         << "    Lives: " << game.player.lives
         << "    Coins left: " << countCoins(game.maze)
         << "\n\n  Controls: W A S D / Arrow Keys | Q: Quit\n";

    cout.flush();
}
