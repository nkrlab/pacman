// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

// Some variables that you may want to change


#ifndef PACMAN_CLIENT_SRC_PACMAN_APP_H_
#define PACMAN_CLIENT_SRC_PACMAN_APP_H_

#include <string>

#include "src/pacman_constants.h"


// Attribute Change operation
void OnChangeGamePoints(const int kGamePoints);
void OnChangeGhostsInARow(const int kGhostsInARow);
void OnChangeLeftPellets(const int kLeftPellets);
void OnChangeLevelNumber(const int kLevelNumber);
void OnChangeRandInternal(const int kRandInternal);
void OnChangeRemainLives(const int kRemainLives);
void OnChangeTimeLeft(const int kTimeLeft);
void OnChangeInvincible(const int kInvincible);
void OnChangeLocations(const std::string &kLocations);
void OnChangeLevel(const std::string &kLevel);
void OnChangeCharactersLives(const std::string &kCharactersLives);
void OnChangeExitMessage(const std::string &kExitMessage);


#endif  // PACMAN_CLIENT_SRC_PACMAN_APP_H_
