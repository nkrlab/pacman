// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

// Some variables that you may want to change


#ifndef CLIENT_SRC_PACMAN_RENDER_H_
#define CLIENT_SRC_PACMAN_RENDER_H_

#include <string>
#include <vector>

#include "src/pacman_constants.h"
#include "src/pacman_util.h"


enum LobbyExitCode { kShowRooms = 0, kMakeRoom, kMakeRoomDuel, kMakeRoomSingle,
                     kJoinRoom, kEscapeLobby };

// Make ncurses windows
void CreateWindows(const int kXStart, const int kYStart,
                   const int kXWidth, const int kYHeight);

// Show introduction screen and menu
void LoginScreen();

// Show lobby room list..
// Return value means true -> make room, false -> refresh
LobbyExitCode LobbyScreen(std::vector<RoomInfo> room_list);

// Show make room screen
bool MakeRoomScreen();

// Join room
int JoinRoomScreen(std::vector<RoomInfo> room_list);

// Draw my window
void DrawMyWindow();

// Draw other window
void DrawOtherWindow();

// Show ghost die
void ShowGhostDie(const int kXPos, const int kYPos,
                  const int kGhostInARow, const bool is_my_play);

// Show pacman die
void ShowPacmanDie(const int kXPos, const int kYPos, const bool is_my_play);

// Destroy ncurses windows
void DestroyWindows();

// Clear other ncurses windows
void ClearOtherWindow();

// Clear my ncurses windows
void ClearMyWindow();


#endif  // CLIENT_SRC_PACMAN_RENDER_H_
