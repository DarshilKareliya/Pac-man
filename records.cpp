#include "game.h"
#include <fstream>
#include <algorithm>
#include <cstdlib>

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

    cout << "========== LEADERBOARD ==========\n\n";

    if (scores.empty()) {
        cout << "No records available yet.\n";
    } else {
        cout << "Rank\tPlayer\t\tScore\n";
        cout << "---------------------------------\n";

        int limit = min(10, static_cast<int>(scores.size()));

        for (int i = 0; i < limit; i++) {
            cout << i + 1 << "\t"
                 << scores[i].first << "\t\t"
                 << scores[i].second << '\n';
        }
    }

    cout << '\n';
}
