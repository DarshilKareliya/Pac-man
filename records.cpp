#include "game.h"
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <iomanip>

void saveScore(const string& name, int score) {
    ofstream file("scores.txt", ios::app);

    if (file.is_open()) {
        file << name << "|" << score << '\n';
        file.close();
    } else {
        cout << "Unable to save score.\n";
    }
}

void showLeaderboard() {
    system("cls");

    ifstream file("scores.txt");
    vector<pair<string, int>> scores;
    string line;

    while (getline(file, line)) {
        size_t separator = line.find('|');

        if (separator == string::npos) {
            continue;
        }

        string name = line.substr(0, separator);
        int score = 0;

        try {
            score = stoi(line.substr(separator + 1));
        } catch (...) {
            continue;
        }

        scores.push_back(make_pair(name, score));
    }

    file.close();

    sort(scores.begin(), scores.end(),
        [](const pair<string, int>& first,
           const pair<string, int>& second) {
            return first.second > second.second;
        });

    const string border = "+------+--------------------------+----------+";

    cout << "\n" << border << '\n';
    cout << "| " << left << setw(42) << "PAC-MAN LEADERBOARD" << " |\n";
    cout << border << '\n';
    cout << "| RANK | PLAYER                   | SCORE    |\n";
    cout << border << '\n';

    if (scores.empty()) {
        cout << "| " << left << setw(42) << "No records available yet." << " |\n";
    } else {
        int limit = min(10, static_cast<int>(scores.size()));

        for (int i = 0; i < limit; i++) {
            string displayName = scores[i].first;
            if (displayName.size() > 24) {
                displayName = displayName.substr(0, 21) + "...";
            }

            cout << "| " << left << setw(4) << (i + 1)
                 << " | " << left << setw(24) << displayName
                 << " | " << right << setw(8) << scores[i].second
                 << " |\n";
        }
    }

    cout << border << "\n\n";
}
