PAC-MAN - C++ COLLEGE PROJECT

Enrollment_no: 02, 03
Names: Darshil Kareliya, Mayank Dama
============================

FILES
-----
game.cpp     Main function and game loop (Mayank Dama)
player.cpp   Player input and movement (Mayank Dama)
ghost.cpp    Automatic ghost movement (Darshil Kareliya)
maze.cpp     Maze creation, display and wall detection (Darshil Kareliya)
menu.cpp     Main menu, instructions and player name (Darshil Kareliya)
records.cpp  Score saving and leaderboard (Mayank Dama)
game.h       Shared declarations used by all modules (Darshil Kareliya)

REQUIREMENTS
------------
- Windows
- GCC / MinGW
- C++11 or later

COMPILE
-------
g++ game.cpp player.cpp ghost.cpp maze.cpp menu.cpp records.cpp -o PacMan.exe -std=c++11

RUN
---
PacMan.exe

CONTROLS
--------
W / Up Arrow     Move up
A / Left Arrow   Move left
S / Down Arrow   Move down
D / Right Arrow  Move right
Q                Quit current game

NOTES
-----
The game uses Windows console functions and is intended for Windows.
Scores are stored in scores.txt in the folder from which the game runs.

This is a starter MVP implementation. Compile and test the integrated
project before submitting. The maze layout and all ghost/player spawn
positions should be checked during testing.