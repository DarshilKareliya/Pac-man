#include "game.h"
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <sstream>

void saveScore(const string& name, const GameState& game) {
    // Check the previous best score before appending the new local record.
    int previousBest = 0;
    ifstream previous("scores.txt");
    string oldLine;
    while (getline(previous, oldLine)) {
        stringstream row(oldLine);
        string oldName, scoreText;
        if (getline(row, oldName, '|') && getline(row, scoreText, '|')) {
            try { previousBest = max(previousBest, stoi(scoreText)); }
            catch (...) { }
        }
    }
    previous.close();

    // The same local file stores leaderboard entries and player statistics.
    ofstream out("scores.txt", ios::app);
    if (out) {
        out << name << "|" << game.player.score << "|"
            << game.player.coinsCollected << "|"
            << game.player.ghostsEaten << "|"
            << game.player.levelsCompleted << "\n";
        out.close();
        if (game.player.score > previousBest)
            cout << "\n🏆 NEW ALL-TIME HIGH SCORE! Congratulations!\n";
    } else {
        cout << "Warning: scores.txt could not be written.\n";
    }
}

void showLeaderboard() {
    system("cls");
    ifstream in("scores.txt");
    vector<pair<string, int> > scores;
    string line;

    while (getline(in, line)) {
        stringstream ss(line);
        string name, scoreText;
        if (!getline(ss, name, '|') || !getline(ss, scoreText, '|')) continue;
        try { scores.push_back(make_pair(name, stoi(scoreText))); }
        catch (...) { continue; }
    }
    sort(scores.begin(), scores.end(),
        [](const pair<string,int>& a, const pair<string,int>& b) {
            return a.second > b.second;
        });

    cout << "\n============== 🏆 TOP 10 LEADERBOARD ==============\n";
    cout << left << setw(7) << "RANK" << setw(25) << "PLAYER" << "SCORE\n";
    cout << "---------------------------------------------------\n";
    if (scores.empty()) cout << "No records yet. Play a game first!\n";
    for (size_t i = 0; i < scores.size() && i < 10; ++i) {
        string name = scores[i].first.substr(0, 23);
        cout << left << setw(7) << (i + 1) << setw(25) << name
             << scores[i].second << "\n";
    }
    cout << "===================================================\n\n";
}

void showStatistics(const string& name, const GameState& game) {
    cout << "\n--------------- PLAYER STATISTICS ---------------\n";
    cout << "Player: " << name << "\n";
    cout << "Coins collected: " << game.player.coinsCollected << "\n";
    cout << "Ghosts eaten: " << game.player.ghostsEaten << "\n";
    cout << "Levels completed: " << game.player.levelsCompleted << "\n";
    cout << "Score: " << game.player.score << "\n";
    cout << "--------------------------------------------------\n";
}
