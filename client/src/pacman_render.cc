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
WINDOW *win_my_game, *win_other_game;
WINDOW *win_my_status, *win_other_status;

// For colors
enum { kWall = 1, kNormal, kPellet, kPowerUp, kGhostWall,
       kGhost1, kGhost2, kGhost3, kGhost4, kBlueGhost, kPacman };

// For input string
enum InputStringType { kID = 1, kPassword, kRoomName, kNumber };


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
  case kNumber:
    y_pos = 22;
    break;
  case kPassword:
    y_pos = 23;
    break;
  case kRoomName:
    y_pos = 20;
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
        if (input_type == kNumber) {
          if (isdigit(ch)) {
            *str += ch;
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
              case kNumber:
                break;
            }
          }
        }
      }
    }

    // between 12~column size clear
    mvwprintw(win_my_game, y_pos, 12, "                ");
    mvwprintw(win_my_game, y_pos, 12, "%s", str->c_str());

    wrefresh(win_my_game);
  }
}


bool GetDuelCheck() {
  bool is_duel = true;
  while (true) {
    // non blocking wait for standard input
    int ch = getch();
    if (ch != ERR) {
      if ((ch == '\n') || (ch == '\t')) {
        break;
      } else if (ch == ' ') {
        is_duel = not is_duel;
      }
    }

    if (is_duel)
      mvwprintw(win_my_game, 21, 12, "(*)");
    else
      mvwprintw(win_my_game, 21, 12, "( )");
    wrefresh(win_my_game);
  }

  return is_duel;
}


void ScreenStatusPush() {
  wclear(win_my_game);
  wrefresh(win_my_game);
  wclear(win_my_status);
  wrefresh(win_my_status);
  wattron(win_my_game, COLOR_PAIR(kNormal));
  nl();
}


void ScreenStatusPop() {
  nonl();
  wclear(win_my_game);
  wrefresh(win_my_game);
  wclear(win_my_status);
  wrefresh(win_my_status);
}

}  // End of anonymous namespace


void CreateWindows(const int kXStart, const int kYStart,
                   const int kXWidth, const int kYHeight) {
  InitCurses();
  CheckScreenSize();

  win_my_game = newwin(kXStart, kYStart, kXWidth, kYHeight);
  win_my_status = newwin(3, 27, 29, 1);

  win_other_game = newwin(kXStart, kYStart, kXWidth, kYStart+1);
  win_other_status = newwin(3, 27, 29, 29);
}


void LoginScreen() {
  ScreenStatusPush();
  curs_set(1);

  mvwprintw(win_my_game, 13, 3, "Enter your ID & Passwd");
  mvwprintw(win_my_game, 22, 1, "ID       : ");
  mvwprintw(win_my_game, 23, 1, "Passward : ");

  std::string id_string, passward_string;
  GetInputString(&id_string, kID);
  GetInputString(&passward_string, kPassword);

  ScreenStatusPop();
  curs_set(0);

  SetAccountId(id_string);
  SendMessageLogin(id_string.c_str());
}


LobbyExitCode LobbyScreen(std::vector<RoomInfo> room_list) {
  LobbyExitCode exit_code = kShowRooms;
  ScreenStatusPush();

  mvwprintw(win_my_game, 3, 10, "Room List");
  mvwprintw(win_my_game, 21, 0, "To Make Room, Press \"C\"");
  mvwprintw(win_my_game, 22, 0, "To Refresh List, Press \"L\"");
  mvwprintw(win_my_game, 23, 0, "To Join Duel Room, Press \"J\"");
  mvwprintw(win_my_game, 23, 0, "To Logout, Press \"Q\"");

  while (true) {
    int ch = getch();
    if (ch != ERR) {
      if ((ch == 'c') || (ch == 'C')) {
        exit_code = kMakeRoom;
        break;
      } else if ((ch == 'l') || (ch == 'L')) {
        exit_code = kShowRooms;
        break;
      } else if ((ch == 'j') || (ch == 'J')) {
        exit_code = kJoinRoom;
        break;
      } else if ((ch == 'q') || (ch == 'Q')) {
        exit_code = kEscapeLobby;
        break;
      }
    }

    for (size_t i = 0; i < room_list.size(); ++i) {
      mvwprintw(win_my_game, 5+i, 6, room_list[i].room_name_.c_str());
    }
    wrefresh(win_my_game);
  }

  ScreenStatusPop();
  return exit_code;
}


bool MakeRoomScreen() {
  ScreenStatusPush();
  curs_set(1);

  mvwprintw(win_my_game, 13, 3, "Enter your room name");
  mvwprintw(win_my_game, 20, 0, "Room Name : ");
  mvwprintw(win_my_game, 21, 0, "Duel Check: (*)");
  mvwprintw(win_my_game, 22, 0, "(Use Spece key)");

  std::string room_name;
  GetInputString(&room_name, kRoomName);
  bool is_duel = GetDuelCheck();
  SendMessageMakeRoom(room_name.c_str(), is_duel);

  ScreenStatusPop();
  curs_set(0);

  return is_duel;
}


int JoinRoomScreen(std::vector<RoomInfo> room_list) {
  ScreenStatusPush();
  curs_set(1);

  mvwprintw(win_my_game, 3, 10, "Room List");
  for (size_t i = 0; i < room_list.size(); ++i) {
    mvwprintw(win_my_game, 5+i, 6, room_list[i].room_name_.c_str());
  }

  mvwprintw(win_my_game, 21, 3, "Enter room number");
  mvwprintw(win_my_game, 22, 0, "Room Number : ");
  wrefresh(win_my_game);

  std::string room_number;
  GetInputString(&room_number, kNumber);
  int value = atoi(room_number.c_str());

  ScreenStatusPop();
  curs_set(0);

  return value;
}


void DrawWindow(const int kLives, const int kLevelNumber,
                const int kPoints, const bool kInvincible,
                const int kTimeLeft,
                const std::vector<std::vector<int> > &kLocate,
                const std::vector<std::vector<int> > &kLevel,
                const bool is_my_play) {
  if (kLevel.size() < static_cast<size_t>(kLevelWidth))
    return;
  if (kLocate.size() < static_cast<size_t>(kChrIndexPacman))
    return;
  int attr = A_NORMAL;

  WINDOW *win_game, *win_status;
  if (is_my_play) {
    win_game = win_my_game;
    win_status = win_my_status;
  } else {
    win_game = win_other_game;
    win_status = win_other_status;
  }

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


void DrawMyWindow() {
  DrawWindow(RemainLives(true), LevelNumber(true), GamePoints(true),
             Invincible(true), TimeLeft(true), Locations(true), Level(true),
             true);
}


void DrawOtherWindow() {
  DrawWindow(RemainLives(false), LevelNumber(false), GamePoints(false),
             Invincible(false), TimeLeft(false), Locations(false),
             Level(false), false);
}


void ShowGhostDie(const int kXPos, const int kYPos,
                  const int kGhostInARow, const bool is_my_play) {
  WINDOW *win_game;
  if (is_my_play) {
    win_game = win_my_game;
  } else {
    win_game = win_other_game;
  }

  mvwprintw(win_game, kXPos, kYPos - 1, "%d",
            (kGhostInARow * 20));
  wrefresh(win_game);
}


void ShowPacmanDie(const int kXPos, const int kYPos, const bool is_my_play) {
  WINDOW *win_game;
  if (is_my_play) {
    win_game = win_my_game;
  } else {
    win_game = win_other_game;
  }

  wattron(win_game, COLOR_PAIR(kPacman));
  mvwprintw(win_game, kXPos, kYPos, "X");
  wrefresh(win_game);
}


void DestroyWindows() {
  endwin();
}


// Clear other ncurses windows
void ClearOtherWindow() {
  wclear(win_other_game);
  wrefresh(win_other_game);
  wclear(win_other_status);
  wrefresh(win_other_status);
}


// Clear my ncurses windows
void ClearMyWindow() {
  wclear(win_my_game);
  wrefresh(win_my_game);
  wclear(win_my_status);
  wrefresh(win_my_status);
}
