// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <vector>

#include "src/pacman_app.h"
#include "src/pacman_constants.h"
#include "src/pacman_render.h"
#include "src/pacman_network.h"


namespace {

// Game Continuous
enum GameContinuous { kNormal = 0, kLevelEnd, kGoLogin };

// How much of a delay is in the game
const int kSpeedOfGame = 170;

// The lastet inputed key
int virtualized_key = 0;

// Game Continuous Variable34
GameContinuous game_continuous = kNormal;

// Temporary stored data from server
int store_game_points = 0;
int store_ghosts_in_a_row = 0;
int store_left_pellets = 0;
int store_level_number = 1;
int store_rand_internal = 0;
int store_remain_lives = 3;
int store_time_left = 0;
bool store_invincible = 0;
std::vector<std::vector<int> > store_locations;
std::vector<std::vector<int> > store_level;

// Variable for sleep
bool need_sleep = false;


void GetInput() {
  static int chtmp = 'a';
  int ch = getch();

  // Buffer input
  if (ch == ERR)
    ch = chtmp;
  if ((ch != 'q') && (ch != 'Q'))
    chtmp = ch;

  switch (ch) {
  case KEY_UP:
  case 'w':
  case 'W':
    virtualized_key = KEY_UP;
    break;
  case KEY_DOWN:
  case 's':
  case 'S':
    virtualized_key = KEY_DOWN;
    break;
  case KEY_LEFT:
  case 'a':
  case 'A':
    virtualized_key = KEY_LEFT;
    break;
  case KEY_RIGHT:
  case 'd':
  case 'D':
    virtualized_key = KEY_RIGHT;
    break;
  case 'q':
  case 'Q':
    game_continuous = kGoLogin;
    break;
  }
}


void Delay() {
  struct timeb t_start, t_current;
  ftime(&t_start);

  // Slow down the game a little bit
  do {
    GetInput();
    ftime(&t_current);
  } while (abs(t_start.millitm - t_current.millitm) < kSpeedOfGame);
}


void MainLoop() {
  const int SLEEP_TIME = 600000;
  RefreshWindow();
  usleep(SLEEP_TIME);

  while (true) {
    // check level end
    if (game_continuous != kNormal)
      break;

    Delay();

    // Net Packet Operate
    HandlingReceivedPacket();

    if (need_sleep) {
      usleep(SLEEP_TIME);
      need_sleep = false;
    }

    // Net Simulate
    DrawWindow(store_remain_lives, store_level_number,
               store_game_points, store_invincible,
               store_time_left, store_locations, store_level);

    // Net Pacman Move
    SendMessage(kPacmanMove, virtualized_key);
  };

  usleep(SLEEP_TIME);
}

}  // End of anonymous namespace


void DoExitProgram(const char *kMessage) {
  // send log out message
  SendMessage(kLogout, kNoUse);

  // terminate network
  NetworkTerminate();

  // destroy windows
  DestroyWindows();

  printf("%s\n", kMessage);
  exit(0);
}

void OnChangeGamePoints(const int kGamePoints) {
  store_game_points = kGamePoints;
}


void OnChangeGhostsInARow(const int kGhostsInARow) {
  store_ghosts_in_a_row = kGhostsInARow;
}


void OnChangeLeftPellets(const int kLeftPellets) {
  store_left_pellets = kLeftPellets;
}


void OnChangeLevelNumber(const int kLevelNumber) {
  if (store_level_number != kLevelNumber) {
    game_continuous = kLevelEnd;
  }
  store_level_number = kLevelNumber;
}


void OnChangeRandInternal(const int kRandInternal) {
  store_rand_internal = kRandInternal;
}


void OnChangeRemainLives(const int kRemainLives) {
  store_remain_lives = kRemainLives;
}


void OnChangeTimeLeft(const int kTimeLeft) {
  store_time_left = kTimeLeft;
}


void OnChangeInvincible(const int kInvincible) {
  if (kInvincible == 1)
    store_invincible = true;
  else
    store_invincible = false;
}


void OnChangeLocations(const std::string &kLocations) {
  for (int i = 0; i < static_cast<int>(store_locations.size()); ++i) {
    store_locations[i].clear();
  }
  store_locations.clear();

  for (int i = 0; i < kChrSize; ++i) {
    std::vector<int> vec_positions;
    for (int j = 0; j < kXYAxis; ++j) {
      int data = kLocations[(i * kXYAxis) + j];
      vec_positions.push_back(data);
    }
    store_locations.push_back(vec_positions);
  }
}


void OnChangeLevel(const std::string &kLevel) {
  for (int i = 0; i < static_cast<int>(store_level.size()); ++i) {
    store_level[i].clear();
  }
  store_level.clear();

  for (int i = 0; i < kLevelWidth; ++i) {
    std::vector<int> column_cell;
    for (int j = 0; j < kLevelHeight; ++j) {
      int index = (i * kLevelHeight) + j;
      int data = kLevel[index];

      column_cell.push_back(data);
    }
    store_level.push_back(column_cell);
  }
}


void OnChangeCharactersLives(const std::string &kCharactersLives) {
  for (int i = 0; i < static_cast<int>(kCharactersLives.size()); ++i) {
    int data = kCharactersLives[i];
    if (data == 0) {
      if (i < kChrIndexPacman) {
        ShowGhostDie(store_locations[i][kIndexX],
                     store_locations[i][kIndexY],
                     store_ghosts_in_a_row/2);
      } else {
        ShowPacmanDie(store_locations[i][kIndexX],
                      store_locations[i][kIndexY]);
      }
      need_sleep = true;
    }
  }
}


void OnChangeExitMessage(const std::string &/*kExitMessage*/) {
  game_continuous = kGoLogin;
}


int main(int /*argc*/, char **/*argv*/) {
  srand(time(NULL));

  // Create Window
  CreateWindows(kLevelWidth, kLevelHeight, 1, 1);

  // Network Initialize();
  NetworkInitialize();

 LOGIN:
  // Login process
  LoginScreen();

  // Load 9 levels, 1 by 1, if you can beat all 9 levels in a row,
  // you're awesome
  for (int i = 1; i < 10; ++i) {
    // Send Level Number
    SendMessage(kLoadLevel, i);

    MainLoop();

    if (game_continuous == kLevelEnd)
      game_continuous = kNormal;
    else if (game_continuous == kGoLogin) {
      SendMessage(kLogout, kNoUse);
      game_continuous = kNormal;
      goto LOGIN;
    }
  }

  DoExitProgram("Good bye!");
}
