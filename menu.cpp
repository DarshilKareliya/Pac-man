#include "game.h"
#include <limits>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif

static void waitBriefly() {
#ifdef _WIN32
    Sleep(160);
#endif
}

MenuChoice showMenu(string& playerName, bool& soundEnabled, int& difficulty) {
    int choice = 0;
    while (true) {
        system("cls");
        cout << "============================================\n";
        cout << "              PAC-MAN V3 \n";
        cout << "============================================\n\n";
        cout << "  1. Start Game\n";
        cout << "  2. Instructions\n";
        cout << "  3. Top 10 Leaderboard\n";
        cout << "  4. Sound: " << (soundEnabled ? "ON" : "OFF") << " (toggle)\n";
        cout << "  5. Difficulty: " << (difficulty == 1 ? "Easy" :
            (difficulty == 2 ? "Medium" : "Hard")) << " (cycle)\n";
        cout << "  6. Exit\n\n";
        cout << "  Choose: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number.\n";
            system("pause");
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                cout << "\nEnter player name: ";
                getline(cin, playerName);
                if (playerName.empty()) playerName = "Player";
                if (playerName.find('|') != string::npos) playerName = "Player";
                system("cls");
                cout << "Welcome, " << playerName << "!\n";
                cout << "Get ready";
                for (int i = 0; i < 3; ++i) { cout << "."; cout.flush(); waitBriefly(); }
                return MenuChoice::START;
            case 2: return MenuChoice::INSTRUCTIONS;
            case 3: return MenuChoice::LEADERBOARD;
            case 4:
                soundEnabled = !soundEnabled;
                cout << "\nSound " << (soundEnabled ? "enabled" : "disabled") << ".\n";
                system("pause");
                break;
            case 5:
                difficulty = difficulty % 3 + 1;
                cout << "\nDifficulty changed.\n";
                system("pause");
                break;
            case 6: return MenuChoice::EXIT;
            default:
                cout << "\nInvalid choice. Select 1-6.\n";
                system("pause");
        }
    }
}

void showInstructions() {
    system("cls");
    cout << "================ HOW TO PLAY ================\n\n";
    cout << "W A S D / Arrow keys : Move\n";
    cout << "P                    : Pause / resume\n";
    cout << "Q                    : Quit current game\n\n";
    cout << "🟡 Collect coins (+10 points).\n";
    cout << "⚡ Power pellets (+50) let you eat ghosts temporarily.\n";
    cout << "🍒 Bonus fruit gives extra points.\n";
    cout << "👻 Avoid ghosts unless power mode is active.\n";
    cout << "❤️ You start with 3 lives.\n";
    cout << "Complete 3 levels to win.\n";
    cout << "Difficulty and sound can be changed in the main menu.\n\n";
    system("pause");
}

