// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

// Some variables that you may want to change


#ifndef CLIENT_SRC_PACMAN_UTIL_H_
#define CLIENT_SRC_PACMAN_UTIL_H_

#include <string>
#include <vector>

/* structure-------------------------------------------------
  world
      game rooms - game room - players - player
                                       - player

                 - game room - players - player
                                       - player

      players    - player        -> 로비의 player들
------------------------------------------------------------*/

void SetAccountId(const std::string &name);
std::vector<std::string> GetRoomList();

void InitializeWorld();
void UpdateFromSerializedBuffer(const std::string &buffer);

int GamePoints();
int GhostsInARow();
bool Invincible();
int LevelNumber();
int RemainLives();
int TimeLeft();
std::vector<std::vector<int> > Locations();
std::vector<std::vector<int> > Level();


#endif  // CLIENT_SRC_PACMAN_UTIL_H_
