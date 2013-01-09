// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <stdlib.h>
#include <curses.h>

#include <unistd.h>
#include <string>

#include "src/pacman_app.h"
#include "src/pacman_network.h"
#include "src/pacman_render.h"
#include "src/pacman_util.h"


namespace {

// For ncurses
WINDOW *win_game;
WINDOW *win_status;

// For colors
enum { kWall = 1, kNormal, kPellet, kPowerUp, kGhostWall,
       kGhost1, kGhost2, kGhost3, kGhost4, kBlueGhost, kPacman };

// For input string
enum InputStringType { kID = 1, kPassword, kRoomName };


// Start up ncurses
void InitCurses() {
  initscr();
  start_color();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  cbreak();
  noecho();
  nonl();

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


void GetInputString(std::string *str, InputStringType input_type) {
  *str = "";
  int y_pos = 0;
  switch (input_type) {
  case kID:
  case kRoomName:
    y_pos = 22;
    break;
  case kPassword:
    y_pos = 23;
    break;
  }

  while (true) {
    int ch;
    // non blocking wait for standard input
    ch = getch();
    if (ch != ERR) {
      if ((ch == '\n') || (ch == '\t')) {
        return;
      } else if (ch == KEY_BACKSPACE) {
        size_t size = str->size();
        if (size >= 1) {
          str->resize(size-1);
        }
      } else {
        if (isprint(ch)) {
          switch (input_type) {
            case kID:
            case kRoomName:
              *str += ch;
            break;
            case kPassword:
              *str += '*';
            break;
          }
        }
      }
    }

    // between 12~column size clear
    mvwprintw(win_game, y_pos, 12, "                ");
    mvwprintw(win_game, y_pos, 12, "%s", str->c_str());

    wrefresh(win_game);
  }
}


void ScreenStatusPush() {
  wclear(win_game);
  wrefresh(win_game);
  wclear(win_status);
  wrefresh(win_status);
  wattron(win_game, COLOR_PAIR(kNormal));
  nl();
}


void ScreenStatusPop() {
  nonl();
  wclear(win_game);
  wrefresh(win_game);
  wclear(win_status);
  wrefresh(win_status);
}

}  // End of anonymous namespace


void CreateWindows(const int kXStart, const int kYStart,
                   const int kXWidth, const int kYHeight) {
  InitCurses();
  CheckScreenSize();

  win_game = newwin(kXStart, kYStart, kXWidth, kYHeight);
  win_status = newwin(3, 27, 29, 1);
}


void LoginScreen() {
  ScreenStatusPush();
  curs_set(1);

  mvwprintw(win_game, 13, 3, "Enter your ID & Passwd");
  mvwprintw(win_game, 22, 1, "ID       : ");
  mvwprintw(win_game, 23, 1, "Passward : ");

  std::string id_string, passward_string;
  GetInputString(&id_string, kID);
  GetInputString(&passward_string, kPassword);

  ScreenStatusPop();
  curs_set(0);

  SetAccountId(id_string);
  SendMessage(kLogin, id_string.c_str(), passward_string.c_str());
}


LobbyExitCode LobbyScreen(std::vector<std::string> kRoomList) {
  LobbyExitCode exit_code = kShowRooms;
  ScreenStatusPush();

  mvwprintw(win_game, 3, 10, "Room List");
  mvwprintw(win_game, 22, 0, "To Make Room, Press \"C\"");
  mvwprintw(win_game, 23, 0, "To Refresh List, Press \"L\"");

  while (true) {
    int ch = getch();
    if (ch != ERR) {
      if ((ch == 'c') || (ch == 'C')) {
        exit_code = kMakeRoom;
        break;
      } else if ((ch == 'l') || (ch == 'L')) {
        exit_code = kShowRooms;
        break;
      } else if ((ch == 'q') || (ch == 'Q')) {
        exit_code = kEscapeLobby;
        break;
      }
    }

    for (size_t i = 0; i < kRoomList.size(); ++i) {
      mvwprintw(win_game, 5+i, 6, kRoomList[i].c_str());
    }
    wrefresh(win_game);
  }

  ScreenStatusPop();
  return exit_code;
}


void MakeRoomScreen() {
  ScreenStatusPush();
  curs_set(1);

  mvwprintw(win_game, 13, 3, "Enter your room name");
  mvwprintw(win_game, 22, 1, "Room Name: ");

  std::string room_name_string;
  GetInputString(&room_name_string, kRoomName);

  SendMessage(kMakeRoomGameStart, room_name_string.c_str(), NULL);

  ScreenStatusPop();
  curs_set(0);
}


void DrawWindow(const int kLives, const int kLevelNumber,
                const int kPoints, const bool kInvincible,
                const int kTimeLeft,
                const std::vector<std::vector<int> > &kLocate,
                const std::vector<std::vector<int> > &kLevel) {
  if (kLevel.size() < static_cast<size_t>(kLevelWidth))
    return;
  if (kLocate.size() < static_cast<size_t>(kChrIndexPacman))
    return;
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
