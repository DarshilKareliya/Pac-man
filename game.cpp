#include "game.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdint>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#endif

using namespace std::chrono;

#ifdef _WIN32
static void writeWavWord(vector<unsigned char>& data, unsigned int value) {
    data.push_back(static_cast<unsigned char>(value & 0xFF));
    data.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));
}

static void writeWavDword(vector<unsigned char>& data, unsigned int value) {
    data.push_back(static_cast<unsigned char>(value & 0xFF));
    data.push_back(static_cast<unsigned char>((value >> 8) & 0xFF));
    data.push_back(static_cast<unsigned char>((value >> 16) & 0xFF));
    data.push_back(static_cast<unsigned char>((value >> 24) & 0xFF));
}

static vector<unsigned char> makeGameWav(int frequency, int durationMs) {
    const unsigned int sampleRate = 22050;
    const unsigned int sampleCount = sampleRate * durationMs / 1000;
    vector<unsigned char> wav;
    wav.reserve(44 + sampleCount);
    wav.insert(wav.end(), {'R','I','F','F'});
    writeWavDword(wav, 36 + sampleCount);
    wav.insert(wav.end(), {'W','A','V','E','f','m','t',' '});
    writeWavDword(wav, 16);       // PCM format chunk size
    writeWavWord(wav, 1);         // PCM
    writeWavWord(wav, 1);         // mono
    writeWavDword(wav, sampleRate);
    writeWavDword(wav, sampleRate);
    writeWavWord(wav, 1);         // block alignment
    writeWavWord(wav, 8);         // 8-bit samples
    wav.insert(wav.end(), {'d','a','t','a'});
    writeWavDword(wav, sampleCount);

    for (unsigned int i = 0; i < sampleCount; ++i) {
        const double t = static_cast<double>(i) / sampleRate;
        const double progress = static_cast<double>(i) / sampleCount;
        // A short arcade-style square wave with a soft attack and release.
        const double envelope = min(1.0, progress * 18.0) * min(1.0, (1.0 - progress) * 12.0);
        const double wave = sin(2.0 * 3.141592653589793 * frequency * t) >= 0.0 ? 1.0 : -1.0;
        const int sample = 128 + static_cast<int>(wave * 38.0 * envelope);
        wav.push_back(static_cast<unsigned char>(sample));
    }
    return wav;
}
#endif

void playGameSound(int soundType, bool enabled) {
    if (!enabled) return;
#ifdef _WIN32
    // Keep each generated WAV in static memory for the full asynchronous playback.
    // SND_NOSTOP prevents rapid coin pickups from cutting off the current sound.
    static vector<unsigned char> sounds[8];
    static bool initialized[8] = {false, false, false, false, false, false, false, false};
    static const int frequencies[8] = {880, 1320, 1560, 190, 1040, 660, 880, 1180};
    static const int durations[8]  = {45, 130, 90, 220, 100, 100, 100, 220};
    if (soundType < 0 || soundType >= 8) soundType = 0;
    if (!initialized[soundType]) {
        sounds[soundType] = makeGameWav(frequencies[soundType], durations[soundType]);
        initialized[soundType] = true;
    }
    PlaySoundA(reinterpret_cast<LPCSTR>(sounds[soundType].data()), NULL,
               SND_MEMORY | SND_ASYNC | SND_NOSTOP | SND_NODEFAULT);
#else
    (void)soundType;
#endif
}

static void playTone(int frequency, int duration, bool enabled) {
    (void)duration;
    int soundType = frequency >= 1150 ? 2 :
                    frequency >= 950 ? 4 :
                    frequency >= 850 ? 6 :
                    frequency >= 600 ? 5 : 3;
    playGameSound(soundType, enabled);
}

// Uses the native Windows timer on MinGW/Windows. This avoids requiring
// MinGW's optional POSIX std::thread implementation.
static void delayMilliseconds(unsigned int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    const clock_t start = clock();
    const clock_t waitTicks = static_cast<clock_t>(milliseconds * CLOCKS_PER_SEC / 1000);
    while (clock() - start < waitTicks) {}
#endif
}

static bool ghostCollision(const GameState& game, size_t& index) {
    for (size_t i = 0; i < game.ghosts.size(); ++i) {
        if (game.ghosts[i].position.row == game.player.position.row &&
            game.ghosts[i].position.col == game.player.position.col) {
            index = i;
            return true;
        }
    }
    return false;
}

static void resetPositions(GameState& game) {
    game.player.position = {1, 1};
    game.player.direction = Direction::NONE;
    const Position starts[] = {{1, 19}, {13, 19}, {13, 1}};
    for (size_t i = 0; i < game.ghosts.size(); ++i) {
        game.ghosts[i].position = starts[i % 3];
        game.ghosts[i].direction = Direction::NONE;
    }
}

static void addGhosts(GameState& game) {
    game.ghosts.clear();
    game.ghosts.push_back({{1,19}, Direction::NONE, {1,19}, 0});
    game.ghosts.push_back({{13,19}, Direction::NONE, {13,19}, 1});
    game.ghosts.push_back({{13,1}, Direction::NONE, {13,1}, 2});
}

static void startCountdown() {
    for (int i = 3; i > 0; --i) {
        system("cls");
        cout << "\n\n              " << i << "\n";
        cout.flush();
        delayMilliseconds(650);
    }
    system("cls");
    cout << "\n\n             GO!\n";
    delayMilliseconds(350);
}

static void pauseScreen() {
    system("cls");
    cout << "\n\n       GAME PAUSED\n\n";
    cout << "       P / ENTER : Resume\n";
    cout << "       Q         : Quit game\n";
}

void runGame(const string& playerName, bool soundEnabled, int difficulty) {
    GameState game;
    game.level = 1;
    game.difficulty = difficulty;
    game.soundEnabled = soundEnabled;
    game.gameOver = false;
    game.won = false;
    game.paused = false;
    game.powerTicks = 0;
    game.fruitTicks = 0;
    game.fruitRow = -1;
    game.fruitCol = -1;
    game.fruitValue = 0;
    game.ghostTicks = 0;
    game.lastFruitCoinCount = -1;
    game.player.position = {1,1};
    game.player.direction = Direction::NONE;
    game.player.lives = 3;
    game.player.score = 0;
    game.player.coinsCollected = 0;
    game.player.ghostsEaten = 0;
    game.player.levelsCompleted = 0;

    srand(static_cast<unsigned int>(time(0)));
    game.maze = createMaze(game.level);
    game.maze.cells[1][1] = ' ';
    addGhosts(game);

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    BOOL oldCursor = cursorInfo.bVisible;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(console, &cursorInfo);
#endif

    startCountdown();
    auto lastPlayerMove = steady_clock::now();
    auto lastGhostMove = steady_clock::now();
    Direction pendingDirection = Direction::NONE;

    while (!game.gameOver) {
        auto now = steady_clock::now();
        Direction input = readPlayerInput();

        if (input == Direction::QUIT) {
            game.gameOver = true;
            break;
        }
        if (input == Direction::NONE) {
            // P and ENTER are handled by readPlayerInput through pause state below.
        }

        // Pause is represented by the dedicated helper key in player.cpp.
        // Toggle is handled by a lightweight console key polling function.
        if (input != Direction::NONE) pendingDirection = input;

        // P toggles pause even when the game is already paused.
        extern bool consumePauseRequest();
        if (consumePauseRequest()) {
            game.paused = !game.paused;
            if (game.paused) pauseScreen();
            else system("cls");
        }

        if (game.paused) {
            delayMilliseconds(30);
            continue;
        }

        long playerDelay = (game.difficulty == 1 ? 165 : (game.difficulty == 2 ? 135 : 110));
        long ghostDelay = (game.difficulty == 1 ? 330 : (game.difficulty == 2 ? 250 : 190));
        playerDelay = max(85L, playerDelay - (game.level - 1) * 5);
        ghostDelay = max(120L, ghostDelay - (game.level - 1) * 8);

        bool changed = false;
        if (duration_cast<milliseconds>(now - lastPlayerMove).count() >= playerDelay) {
            movePlayer(game, pendingDirection);
            pendingDirection = Direction::NONE;
            lastPlayerMove = now;
            changed = true;
        }

        if (game.fruitTicks == 0) {
            game.fruitRow = game.fruitCol = -1;
            game.fruitValue = 0;
        }

        // Spawn a fruit after every 20 coins collected, at a fixed safe path cell.
        if (game.player.coinsCollected > 0 &&
            game.player.coinsCollected % 20 == 0 &&
            game.lastFruitCoinCount != game.player.coinsCollected &&
            game.fruitTicks == 0) {
            game.fruitRow = 5;
            game.fruitCol = 10;
            game.fruitValue = 100 + game.level * 25;
            game.fruitTicks = 90;
            game.lastFruitCoinCount = game.player.coinsCollected;
        }

        if (duration_cast<milliseconds>(now - lastGhostMove).count() >= ghostDelay) {
            updateGhosts(game);
            // Timers advance with game ticks rather than every render-loop pass.
            if (game.powerTicks > 0) --game.powerTicks;
            if (game.fruitTicks > 0) --game.fruitTicks;
            lastGhostMove = now;
            changed = true;
        }

        // Handle player collision with ghosts. Power mode lets Pac-Man eat them.
        size_t hitIndex = 0;
        if (ghostCollision(game, hitIndex)) {
            if (game.powerTicks > 0) {
                game.player.score += 200;
                ++game.player.ghostsEaten;
                game.ghosts[hitIndex].position = game.ghosts[hitIndex].home;
                playTone(1200, 90, game.soundEnabled);
            } else {
                --game.player.lives;
                playTone(180, 260, game.soundEnabled);
                if (game.player.lives <= 0) {
                    game.gameOver = true;
                } else {
                    resetPositions(game);
                    pendingDirection = Direction::NONE;
                    delayMilliseconds(500);
                }
            }
            changed = true;
        }

        if (game.fruitTicks > 0 && game.player.position.row == game.fruitRow &&
            game.player.position.col == game.fruitCol) {
            game.player.score += game.fruitValue;
            game.fruitTicks = 0;
            game.fruitRow = game.fruitCol = -1;
            playTone(1000, 100, game.soundEnabled);
            changed = true;
        }

        if (countCoins(game.maze) == 0) {
            ++game.player.levelsCompleted;
            if (game.level >= 3) {
                game.won = true;
                game.gameOver = true;
            } else {
                ++game.level;
                game.maze = createMaze(game.level);
                game.maze.cells[1][1] = ' ';
                game.powerTicks = 0;
                game.fruitTicks = 0;
                game.fruitRow = game.fruitCol = -1;
                resetPositions(game);
                startCountdown();
                lastPlayerMove = lastGhostMove = steady_clock::now();
                changed = true;
            }
        }

        if (changed) drawMaze(game);
        delayMilliseconds(8);
    }

    system("cls");
    cout << "========================================\n";
    cout << "              PAC-MAN V3\n";
    cout << "========================================\n\n";
    cout << "Player: " << playerName << "\n";
    cout << "Final score: " << game.player.score << "\n";
    cout << "Lives remaining: " << game.player.lives << "\n";
    cout << "Level reached: " << game.level << "\n";
    cout << "Coins collected: " << game.player.coinsCollected << "\n";
    cout << "Ghosts eaten: " << game.player.ghostsEaten << "\n";
    cout << "Levels completed: " << game.player.levelsCompleted << "\n\n";
    if (game.won) {
        cout << "🏆 Congratulations! You completed all 3 levels!\n";
        playTone(700, 120, game.soundEnabled);
        playTone(900, 120, game.soundEnabled);
        playTone(1100, 250, game.soundEnabled);
    } else {
        cout << "GAME OVER - Thanks for playing!\n";
        playTone(300, 180, game.soundEnabled);
        playTone(220, 300, game.soundEnabled);
    }

    saveScore(playerName, game);
    showStatistics(playerName, game);

#ifdef _WIN32
    cursorInfo.bVisible = oldCursor;
    SetConsoleCursorInfo(console, &cursorInfo);
#endif
    cout << "\n";
    system("pause");
}

int main() {
#ifdef _WIN32
    // Automatically select UTF-8 before printing the menu and emoji.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    string playerName;
    bool soundEnabled = true;
    int difficulty = 2;

    while (true) {
        MenuChoice choice = showMenu(playerName, soundEnabled, difficulty);
        switch (choice) {
            case MenuChoice::START:
                runGame(playerName, soundEnabled, difficulty);
                break;
            case MenuChoice::INSTRUCTIONS:
                showInstructions();
                break;
            case MenuChoice::LEADERBOARD:
                showLeaderboard();
                system("pause");
                break;
            case MenuChoice::EXIT:
                cout << "\nThanks for playing Pac-Man V3!\n";
                return 0;
        }
    }
}
