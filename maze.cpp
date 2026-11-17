#include "game.h"
#include <iomanip>
#include <sstream>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif

Maze createMaze(int level) {
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

    // Place power pellets on open paths.
    maze.cells[1][3] = 'o';
    maze.cells[1][17] = 'o';
    maze.cells[13][3] = 'o';
    maze.cells[13][17] = 'o';

    // Each level changes the wall layout by opening new passages.
    // These shortcuts connect existing corridors and do not isolate spawns.
    if (level >= 2) {
        maze.cells[2][3] = '.';
        maze.cells[2][17] = '.';
        maze.cells[10][3] = '.';
        maze.cells[10][17] = '.';
        maze.cells[7][10] = 'o';
    }
    if (level >= 3) {
        maze.cells[4][3] = '.';
        maze.cells[4][17] = '.';
        maze.cells[12][3] = '.';
        maze.cells[12][17] = '.';
        maze.cells[9][3] = 'o';
        maze.cells[9][17] = 'o';
    }
    return maze;
}

bool isWall(const Maze& maze, Position p) {
    if (p.row < 0 || p.row >= static_cast<int>(maze.cells.size())) return true;
    if (p.col < 0 || p.col >= static_cast<int>(maze.cells[p.row].size())) return true;
    return maze.cells[p.row][p.col] == '#';
}

int countCoins(const Maze& maze) {
    int total = 0;
    for (const string& row : maze.cells)
        for (char cell : row)
            if (cell == '.' || cell == 'o') ++total;
    return total;
}

void drawMaze(const GameState& game) {
    vector<string> screen = game.maze.cells;
    if (game.fruitTicks > 0 && game.fruitRow >= 0 &&
        game.fruitRow < static_cast<int>(screen.size()) &&
        game.fruitCol >= 0 && game.fruitCol < static_cast<int>(screen[game.fruitRow].size()))
        screen[game.fruitRow][game.fruitCol] = 'F';

    for (const Ghost& ghost : game.ghosts)
        if (ghost.position.row >= 0 && ghost.position.row < static_cast<int>(screen.size()) &&
            ghost.position.col >= 0 && ghost.position.col < static_cast<int>(screen[ghost.position.row].size()))
            screen[ghost.position.row][ghost.position.col] = 'G';
    if (game.player.position.row >= 0 &&
        game.player.position.row < static_cast<int>(screen.size()) &&
        game.player.position.col >= 0 &&
        game.player.position.col < static_cast<int>(screen[game.player.position.row].size()))
        screen[game.player.position.row][game.player.position.col] = 'P';

    ostringstream scoreLine;
    scoreLine << "Score: " << setw(6) << game.player.score
              << "   Lives: " << game.player.lives
              << "   Level: " << game.level
              << "   Coins: " << countCoins(game.maze);
    ostringstream statusLine;
    statusLine << "Difficulty: " << (game.difficulty == 1 ? "Easy" :
        (game.difficulty == 2 ? "Medium" : "Hard"))
               << "   Sound: " << (game.soundEnabled ? "ON" : "OFF")
               << "   Power: " << (game.powerTicks > 0 ? "ACTIVE" : "OFF");

    vector<string> lines;
    lines.push_back("+--------------------------------------------------+");
    lines.push_back("|                    PAC-MAN V3                    |");
    lines.push_back("+--------------------------------------------------+");
    lines.push_back(scoreLine.str());
    lines.push_back(statusLine.str());
    lines.push_back("");

    for (const string& row : screen) {
        string renderedRow;
        for (char cell : row) {
            switch (cell) {
                case '#': renderedRow += "🟦"; break;
                case '.': renderedRow += "🟡"; break;
                case 'o': renderedRow += "⚡"; break;
                case 'P': renderedRow += "😮"; break;
                case 'G': renderedRow += (game.powerTicks > 0 ? "😨" : "👻"); break;
                case 'F': renderedRow += "🍒"; break;
                default: renderedRow += "  "; break;
            }
        }
        lines.push_back(renderedRow);
    }
    lines.push_back("");
    lines.push_back("WASD / Arrow keys: Move | P: Pause | Q: Quit");

    // Center each frame in the *visible* console window. The widest UI line
    // is 52 columns; the 21-tile maze is rendered at about 42 columns.
    const int contentWidth = 52;
    const int contentHeight = static_cast<int>(lines.size());
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(console, &info)) {
        const int windowWidth = info.srWindow.Right - info.srWindow.Left + 1;
        const int windowHeight = info.srWindow.Bottom - info.srWindow.Top + 1;
        const int leftPadding = windowWidth > contentWidth ? (windowWidth - contentWidth) / 2 : 0;
        const int topPadding = windowHeight > contentHeight ? (windowHeight - contentHeight) / 2 : 0;
        for (size_t i = 0; i < lines.size(); ++i) {
            const int lineWidth = (i >= 6 && i < 21) ? 42 : static_cast<int>(lines[i].size());
            const int linePadding = leftPadding + (contentWidth - lineWidth) / 2;
            COORD cursor;
            cursor.X = static_cast<SHORT>(info.srWindow.Left + linePadding);
            cursor.Y = static_cast<SHORT>(info.srWindow.Top + topPadding + static_cast<int>(i));
            SetConsoleCursorPosition(console, cursor);
            cout << lines[i] << string(static_cast<size_t>(contentWidth - lineWidth), ' ');
        }
        // Keep the cursor out of the visible game area to avoid blinking.
        COORD cursor;
        cursor.X = info.srWindow.Left;
        cursor.Y = info.srWindow.Top;
        SetConsoleCursorPosition(console, cursor);
    } else {
        for (const string& line : lines) cout << line << "\n";
    }
#else
    cout << "\033[H";
    for (const string& line : lines) cout << line << "\033[K\n";
#endif
    cout.flush();
}

