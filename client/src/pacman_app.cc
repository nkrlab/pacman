// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include "src/pacman_app.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <time.h>

#include <boost/asio.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <curses.h>
#include <json_spirit.h>
#include <map>
#include <string>
#include <vector>

#include "src/pacman_constants.h"
#include "src/pacman_render.h"
#include "src/pacman_network.h"
#include "src/pacman_util.h"


namespace {

// Game Continuous
enum GameContinuous { kNormal = 0, kLevelEnd, kEscape, kWaitingRoomList };

// How much of a delay is in the game
const int kSpeedOfGame = 170;

// The lastet inputed key
int virtualized_key = 0;

// Game Continuous Variable
GameContinuous game_continuous = kNormal;

// Variable for sleep
bool need_sleep = false;

// Current level number
int current_level = 0;


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
    game_continuous = kEscape;
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

  game_continuous = kNormal;
  while (true) {
    // check level end
    if (game_continuous != kNormal) {
      if (game_continuous == kEscape) {
        // request game end and leave room
        SendMessage(kGameEndLeaveRoom, kNoUse);
      }
      break;
    }

    Delay();

    // Net Packet Operate
    HandlingReceivedPacket();

    if (need_sleep) {
      usleep(SLEEP_TIME);
      need_sleep = false;
    }

    // Net Simulate
    DrawWindow(RemainLives(), LevelNumber(), GamePoints(), Invincible(),
               TimeLeft(), Locations(), Level());

    // Net Pacman Move
    SendMessage(kPacmanMove, virtualized_key);
  };

  usleep(SLEEP_TIME);
}


void WaitingForRoomList() {
  game_continuous = kWaitingRoomList;

  while (true) {
    // Net Packet Operate
    HandlingReceivedPacket();

    if (game_continuous == kNormal)
      break;
  }
}


void LobbyProcess() {
  WaitingForRoomList();

  LobbyExitCode exit_code = kShowRooms;
  while (true) {
    exit_code = LobbyScreen(GetRoomList());
    if (exit_code != kShowRooms)
      break;

    SendMessage(kShowRoomList, kNoUse);
    WaitingForRoomList();
  }

  if (exit_code == kMakeRoom) {
    game_continuous = kNormal;
    MakeRoomScreen();
  } else if (exit_code == kEscapeLobby) {
    game_continuous = kEscape;
  }
}


void GameLevelLoop() {
  for (volatile int i = 1; i < 10; ++i) {
    // Send Level Number
    SendMessage(kLoadLevel, i);
    current_level = i;

    // game main loop
    MainLoop();

    if (game_continuous == kLevelEnd) {
      game_continuous = kNormal;
      // clear all fun object
      InitializeWorld();
    } else if (game_continuous == kEscape) {
      game_continuous = kNormal;
      break;
    }
  }
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


void OnChangeExitMessage(const std::string &/*exit_message*/) {
  game_continuous = kEscape;
}


void SetNeedSleep() {
  need_sleep = true;
}


void ReceivedRoomList() {
  game_continuous = kNormal;
}


void ReceivedLevelNumber(volatile int level_number) {
  if (current_level != level_number) {
    game_continuous = kLevelEnd;
  }
}


int main(int /*argc*/, char **/*argv*/) {
  srand(time(NULL));

  // Create Window
  CreateWindows(kLevelWidth, kLevelHeight, 1, 1);

  // Network Initialize();
  NetworkInitialize();

  game_continuous = kEscape;
  // Client loop
  while (true) {
    // check game continuous
    if (game_continuous != kEscape)
      break;

    // Login screen
    LoginScreen();

    // clear all fun object
    InitializeWorld();

    while (true) {
      // Lobby process
      LobbyProcess();

      // check game continuous
      if (game_continuous == kEscape) {
        SendMessage(kLogout, kNoUse);
        break;
      }

      // Game level loop
      GameLevelLoop();
    }
  }

  DoExitProgram("Good bye!");
}
