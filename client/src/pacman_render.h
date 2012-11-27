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


// Make ncurses windows
void CreateWindows(const int kXStart, const int kYStart,
                   const int kXWidth, const int kYHeight);

// Show introduction screen and menu
void LoginScreen();

// Show lobby room list..
// Return value means true -> make room, false -> refresh
bool LobbyScreen(std::vector<std::string> kRoomList);

// Show make room screen
void MakeRoomScreen();

// Refresh display
void DrawWindow(const int kLives, const int kLevelNumber,
                const int kPoints, const bool kInvincible,
                const int kTimeLeft,
                const std::vector<std::vector<int> > &kLocate,
                const std::vector<std::vector<int> > &kLevel);

// Show ghost die
void ShowGhostDie(const int kXPos, const int kYPos,
                  const int kGhostInARow);

// Show pacman die
void ShowPacmanDie(const int kXPos, const int kYPos);

// Refresh ncurses windows
void RefreshWindow();

// Destroy ncurses windows
void DestroyWindows();


#endif  // CLIENT_SRC_PACMAN_RENDER_H_
