#include "game.h"
#include <limits>
#include <cstdlib>

MenuChoice showMenu(string& playerName) {
    int choice;

    while (true) {
        system("cls");

        cout << "============================\n";
        cout << "       PAC-MAN ARCADE       \n";
        cout << "============================\n\n";
        cout << "1. Start Game\n";
        cout << "2. Instructions\n";
        cout << "3. Leaderboard\n";
        cout << "4. Exit\n";
        cout << "\nEnter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter a number.\n";
            system("pause");
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                cout << "\nEnter your name: ";
                getline(cin, playerName);

                if (playerName.empty()) {
                    playerName = "Player";
                }

                return MenuChoice::START;

            case 2:
                return MenuChoice::INSTRUCTIONS;

            case 3:
                return MenuChoice::LEADERBOARD;

            case 4:
                return MenuChoice::EXIT;

            default:
                cout << "\nInvalid choice.\n";
                system("pause");
        }
    }
}

void showInstructions() {
    system("cls");

    cout << "========== INSTRUCTIONS ==========\n\n";
    cout << "1. Use W A S D or arrow keys to move.\n";
    cout << "2. Collect coins to earn 10 points each.\n";
    cout << "3. Avoid the ghosts.\n";
    cout << "4. You have three lives.\n";
    cout << "5. Collect all coins to win.\n";
    cout << "6. Press Q during the game to quit.\n\n";

    system("pause");
}
