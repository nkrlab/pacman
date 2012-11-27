
// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <stdlib.h>
#include <curses.h>
#include <unistd.h>

#include "src/pacman_render.h"


namespace {

// For ncurses
WINDOW *win_game;
WINDOW *win_status;

// For colors
enum { kWall = 1, kNormal, kPellet, kPowerUp, kGhostWall,
       kGhost1, kGhost2, kGhost3, kGhost4, kBlueGhost, kPacman };


// Start up ncurses
void InitCurses() {
  initscr();
  start_color();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  nonl();
  cbreak();
  noecho();

  init_pair(kNormal,    COLOR_WHITE,   COLOR_BLACK);
  init_pair(kWall,      COLOR_WHITE,   COLOR_WHITE);
  init_pair(kPellet,    COLOR_WHITE,   COLOR_BLACK);
  init_pair(kPowerUp,   COLOR_BLUE,    COLOR_BLACK);
  init_pair(kGhostWall, COLOR_WHITE,   COLOR_CYAN);
  init_pair(kGhost1,    COLOR_RED,     COLOR_BLACK);
  init_pair(kGhost2,    COLOR_CYAN,    COLOR_BLACK);
  init_pair(kGhost3,    COLOR_MAGENTA, COLOR_BLACK);
  init_pair(kGhost4,    COLOR_YELLOW,  COLOR_BLACK);
  init_pair(kBlueGhost, COLOR_BLUE,    COLOR_RED);
  init_pair(kPacman,    COLOR_YELLOW,  COLOR_BLACK);
}


// Make sure the window is big enough
void CheckScreenSize() {
  int width, height;
  getmaxyx(stdscr, height, width);

  if ((height < 32) || (width < 29)) {
    endwin();
    fprintf(stderr, "\nSorry.\n");
    fprintf(stderr, "To play Pacman for Console, your console");
    fprintf(stderr, "window must be at least 32x29\n");
    fprintf(stderr, "Please resize your window/resolution and");
    fprintf(stderr, "re-run the game.\n\n");
    exit(0);
  }
}

}  // End of anonymous namespace


void CreateWindows(const int kXStart, const int kYStart,
                   const int kXWidth, const int kYHeight) {
  InitCurses();
  CheckScreenSize();

  win_game = newwin(kXStart, kYStart, kXWidth, kYHeight);
  win_status = newwin(3, 27, 29, 1);
}


void IntroScreen() {
  int ghost_speed = 23;

  getch();
  getch();
  getch();

  mvwprintw(win_game, 20, 8, "Press any key...");

  // Scroll Pacman to middle of screen
  for (int i = 0; i < 13; ++i) {
    if (getch() != ERR)
      return;

    wattron(win_game, COLOR_PAIR(kPacman));
    mvwprintw(win_game, 8, i, " C");
    wrefresh(win_game);
    usleep(100000);
  }

  // Show "Pacman"
  wattron(win_game, COLOR_PAIR(kPacman));
  mvwprintw(win_game, 8, 12, "PACMAN");
  wrefresh(win_game);
  usleep(1000000);

  // Ghosts Chase Pacman
  for (int i = 0; i < 23; ++i) {
    if (getch() != ERR)
      return;
    wattron(win_game, COLOR_PAIR(kPellet));
    mvwprintw(win_game, 13, 23, "*");
    wattron(win_game, COLOR_PAIR(kPacman));
    mvwprintw(win_game, 13, i, " C");
    wattron(win_game, COLOR_PAIR(kGhost1));
    mvwprintw(win_game, 13, i-3, " &");
    wattron(win_game, COLOR_PAIR(kGhost3));
    mvwprintw(win_game, 13, i-5, " &");
    wattron(win_game, COLOR_PAIR(kGhost2));
    mvwprintw(win_game, 13, i-7, " &");
    wattron(win_game, COLOR_PAIR(kGhost4));
    mvwprintw(win_game, 13, i-9, " &");
    wrefresh(win_game);
    usleep(100000);
  }

  usleep(150000);

  // Pacman Chases Ghosts
  for (int i = 25; i > 2; --i) {
    if (getch() != ERR)
      return;
    wattron(win_game, COLOR_PAIR(kPellet));
    mvwprintw(win_game, 13, 23, " ");

    // Make ghosts half as fast
    if (i%2)
      --ghost_speed;

    wattron(win_game, COLOR_PAIR(kBlueGhost));
    mvwprintw(win_game, 13, ghost_speed-9, "& & & &");
    wattron(win_game, COLOR_PAIR(kPacman));
    mvwprintw(win_game, 13, ghost_speed-9+1, " ");
    wattron(win_game, COLOR_PAIR(kPacman));
    mvwprintw(win_game, 13, ghost_speed-9+3, " ");
    wattron(win_game, COLOR_PAIR(kPacman));
    mvwprintw(win_game, 13, ghost_speed-9+5, " ");
    wattron(win_game, COLOR_PAIR(kPacman));
    mvwprintw(win_game, 13, ghost_speed-9+7, " ");
    wattron(win_game, COLOR_PAIR(kPacman));
    mvwprintw(win_game, 13, i-3, "C          ");
    wattron(win_game, COLOR_PAIR(kPellet));
    mvwprintw(win_game, 13, 23, " ");
    wrefresh(win_game);
    usleep(100000);
  }
}


void DrawWindow(const int kLives, const int kLevelNumber,
                const int kPoints, const bool kInvincible,
                const int kTimeLeft,
                const std::vector<std::vector<int> > &kLocate,
                const std::vector<std::vector<int> > &kLevel) {
  int attr = A_NORMAL;

  // Display level array
  for (int i = 0; i < kLevelWidth; ++i) {
    for (int j = 0; j < kLevelHeight; ++j) {
      char chr = ' ';

      switch (kLevel[i][j]) {
        case kLevelCellEmpty:
          chr = ' ';
          attr = A_NORMAL;
          wattron(win_game, COLOR_PAIR(kNormal));
          break;
        case kLevelCellWall:
          chr = ' ';
          attr = A_NORMAL;
          wattron(win_game, COLOR_PAIR(kWall));
          break;
        case kLevelCellPellet:
          chr = '.';
          attr = A_NORMAL;
          wattron(win_game, COLOR_PAIR(kPellet));
          break;
        case kLevelCellPowerUp:
          chr = '*';
          attr = A_BOLD;
          wattron(win_game, COLOR_PAIR(kPowerUp));
          break;
        case kLevelCellGhostWall:
          chr = ' ';
          attr = A_NORMAL;
          wattron(win_game, COLOR_PAIR(kGhostWall));
          break;
        }
      mvwaddch(win_game, i, j, chr | attr);
    }
  }

  // Display number of lives, score, and level
  attr = A_NORMAL;
  wmove(win_status, 1, 1);
  wattron(win_status, COLOR_PAIR(kPacman));
  for (int i = 0; i < kLives; ++i)
    wprintw(win_status, "C ");
  wprintw(win_status, "  ");
  wattron(win_status, COLOR_PAIR(kNormal));
  mvwprintw(win_status, 2, 2, "Level: %d     Score: %d ",
            kLevelNumber, kPoints);
  wrefresh(win_status);

  // Display ghosts
  if (!kInvincible) {
    wattron(win_game, COLOR_PAIR(kGhost1));
    mvwaddch(win_game, kLocate[kChrIndexGhost0][kIndexX],
             kLocate[kChrIndexGhost0][kIndexY], '&');
    wattron(win_game, COLOR_PAIR(kGhost2));
    mvwaddch(win_game, kLocate[kChrIndexGhost1][kIndexX],
             kLocate[kChrIndexGhost1][kIndexY], '&');
    wattron(win_game, COLOR_PAIR(kGhost3));
    mvwaddch(win_game, kLocate[kChrIndexGhost2][kIndexX],
             kLocate[kChrIndexGhost2][kIndexY], '&');
    wattron(win_game, COLOR_PAIR(kGhost4));
    mvwaddch(win_game, kLocate[kChrIndexGhost3][kIndexX],
             kLocate[kChrIndexGhost3][kIndexY], '&');
  } else {  // OR display vulnerable ghosts
    wattron(win_game, COLOR_PAIR(kBlueGhost));
    mvwaddch(win_game, kLocate[kChrIndexGhost0][kIndexX],
             kLocate[kChrIndexGhost0][kIndexY], kTimeLeft + '0');
    mvwaddch(win_game, kLocate[kChrIndexGhost1][kIndexX],
             kLocate[kChrIndexGhost1][kIndexY], kTimeLeft + '0');
    mvwaddch(win_game, kLocate[kChrIndexGhost2][kIndexX],
             kLocate[kChrIndexGhost2][kIndexY], kTimeLeft + '0');
    mvwaddch(win_game, kLocate[kChrIndexGhost3][kIndexX],
             kLocate[kChrIndexGhost3][kIndexY], kTimeLeft + '0');
  }

  // Display Pacman
  wattron(win_game, COLOR_PAIR(kPacman));
  mvwaddch(win_game, kLocate[kChrIndexPacman][kIndexX],
           kLocate[kChrIndexPacman][kIndexY], 'C');
  wrefresh(win_game);
}


void ShowGhostDie(const int kXPos, const int kYPos,
                  const int kGhostInARow) {
  mvwprintw(win_game, kXPos, kYPos - 1, "%d",
            (kGhostInARow * 20));
  wrefresh(win_game);
}


void ShowPacmanDie(const int kXPos, const int kYPos) {
  wattron(win_game, COLOR_PAIR(kPacman));
  mvwprintw(win_game, kXPos, kYPos, "X");
  wrefresh(win_game);
}


void RefreshWindow() {
  wrefresh(win_game);
  wrefresh(win_status);
}


void DestroyWindows() {
  endwin();
}
