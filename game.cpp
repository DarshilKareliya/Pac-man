#include "game.h"
#include <windows.h>
#include <chrono>
#include <cstdlib>

using namespace std::chrono;

static bool checkGhostCollision(const GameState& game) {
    for (const Ghost& ghost : game.ghosts) {
        if (ghost.position.row == game.player.position.row &&
            ghost.position.col == game.player.position.col) {
            return true;
        }
    }
    return false;
}

static void resetPositions(GameState& game) {
    game.player.position = {1, 1};
    game.player.direction = Direction::NONE;

    game.ghosts[0].position = {1, 19};
    game.ghosts[1].position = {13, 19};
    game.ghosts[2].position = {13, 1};

    for (Ghost& ghost : game.ghosts) {
        ghost.direction = Direction::NONE;
    }
}

void runGame(const string& playerName) {
    system("cls");

    GameState game;
    game.maze = createMaze();

    game.player.position = {1, 1};
    game.player.direction = Direction::NONE;
    game.player.lives = 3;
    game.player.score = 0;

    // The player's starting cell should not contain a coin.
    game.maze.cells[1][1] = ' ';

    game.ghosts.push_back({{1, 19}, Direction::NONE});
    game.ghosts.push_back({{13, 19}, Direction::NONE});
    game.ghosts.push_back({{13, 1}, Direction::NONE});

    game.gameOver = false;
    game.won = false;

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    const bool oldCursorVisibility = cursorInfo.bVisible;

    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(console, &cursorInfo);

    auto lastPlayerMove = steady_clock::now();
    auto lastGhostMove = steady_clock::now();
    Direction pendingDirection = Direction::NONE;

    drawMaze(game);

    while (!game.gameOver) {
        auto currentTime = steady_clock::now();

        Direction input = readPlayerInput();

        if (input == Direction::QUIT) {
            game.gameOver = true;
            break;
        }

        if (input != Direction::NONE) {
            pendingDirection = input;
        }

        bool screenNeedsUpdate = false;

        auto playerElapsed = duration_cast<milliseconds>(
            currentTime - lastPlayerMove
        ).count();

        if (playerElapsed >= 140) {
            movePlayer(game, pendingDirection);
            pendingDirection = Direction::NONE;
            lastPlayerMove = currentTime;
            screenNeedsUpdate = true;
        }

        auto ghostElapsed = duration_cast<milliseconds>(
            currentTime - lastGhostMove
        ).count();

        if (ghostElapsed >= 260) {
            updateGhosts(game);
            lastGhostMove = currentTime;
            screenNeedsUpdate = true;
        }

        // Check collision after either player or ghost movement.
        if (checkGhostCollision(game)) {
            game.player.lives--;

            if (game.player.lives <= 0) {
                game.gameOver = true;
            } else {
                resetPositions(game);
                pendingDirection = Direction::NONE;
                Sleep(500);
            }

            screenNeedsUpdate = true;
        }

        if (countCoins(game.maze) == 0) {
            game.won = true;
            game.gameOver = true;
        }

        if (screenNeedsUpdate) {
            drawMaze(game);
        }

        Sleep(10);
    }

    system("cls");

    cout << "========== GAME OVER ==========\n\n";
    cout << "Player: " << playerName << '\n';
    cout << "Final score: " << game.player.score << '\n';
    cout << "Lives remaining: " << game.player.lives << '\n';

    if (game.won) {
        cout << "\nCongratulations! You collected all the coins!\n";
    } else {
        cout << "\nThanks for playing!\n";
    }

    saveScore(playerName, game.player.score);

    cursorInfo.bVisible = oldCursorVisibility;
    SetConsoleCursorInfo(console, &cursorInfo);

    cout << '\n';
    system("pause");
}

int main() {
    string playerName;

    while (true) {
        MenuChoice choice = showMenu(playerName);

        switch (choice) {
            case MenuChoice::START:
                runGame(playerName);
                break;

            case MenuChoice::INSTRUCTIONS:
                showInstructions();
                break;

            case MenuChoice::LEADERBOARD:
                showLeaderboard();
                system("pause");
                break;

            case MenuChoice::EXIT:
                cout << "\nThank you for playing Pac-Man!\n";
                return 0;
        }
    }
}
