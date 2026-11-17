#include "game.h"
#include <limits>
#include <cstdlib>
#include <windows.h>

// A short console animation shown after the player enters their name.
// This changes only the presentation; it does not affect game state or timing.
static void showLoadingAnimation(const string& playerName) {
    const string frames[] = {
        "[                    ]",
        "[=====               ]",
        "[==========          ]",
        "[===============     ]",
        "[====================]"
    };

    for (const string& frame : frames) {
        system("cls");
        cout << "+------------------------------------------+\n";
        cout << "|              PAC-MAN ARCADE              |\n";
        cout << "+------------------------------------------+\n\n";
        cout << "  Welcome, " << playerName << "!\n\n";
        cout << "          Preparing your game...\n\n";
        cout << "                 " << frame << "\n\n";
        cout.flush();
        Sleep(180);
    }
    system("cls");
}

MenuChoice showMenu(string& playerName) {
    int choice;

    while (true) {
        system("cls");

        cout << "+==========================================+\n";
        cout << "|               PAC-MAN ARCADE             |\n";
        cout << "+==========================================+\n\n";
        cout << "       .-----------------------------.\n";
        cout << "       |  1.  Start Game             |\n";
        cout << "       |  2.  Instructions           |\n";
        cout << "       |  3.  Leaderboard            |\n";
        cout << "       |  4.  Exit                   |\n";
        cout << "       '-----------------------------'\n";
        cout << "\n  Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n  Please enter a number.\n";
            system("pause");
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                cout << "\n  Enter your name: ";
                getline(cin, playerName);

                if (playerName.empty()) {
                    playerName = "Player";
                }

                showLoadingAnimation(playerName);
                return MenuChoice::START;

            case 2:
                return MenuChoice::INSTRUCTIONS;

            case 3:
                return MenuChoice::LEADERBOARD;

            case 4:
                return MenuChoice::EXIT;

            default:
                cout << "\n  Invalid choice. Please choose 1-4.\n";
                system("pause");
        }
    }
}

void showInstructions() {
    system("cls");

    cout << "+==========================================+\n";
    cout << "|                HOW TO PLAY               |\n";
    cout << "+==========================================+\n\n";
    cout << "  1. Use W A S D or arrow keys to move.\n";
    cout << "  2. Collect coins to earn 10 points each.\n";
    cout << "  3. Avoid the ghosts.\n";
    cout << "  4. You have three lives.\n";
    cout << "  5. Collect all coins to win.\n";
    cout << "  6. Press Q during the game to quit.\n\n";
    cout << "+------------------------------------------+\n\n";

    system("pause");
}

