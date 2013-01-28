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

struct RoomInfo {
  std::string room_name_;
  bool duel_;
};

void SetAccountId(const std::string &name);
std::vector<RoomInfo> GetRoomList();

void InitializeWorld();
void UpdateFromSerializedBuffer(const std::string &buffer);

int GamePoints(bool is_my_player);
int GhostsInARow(bool is_my_player);
bool Invincible(bool is_my_player);
int LevelNumber(bool is_my_player);
int RemainLives(bool is_my_player);
int TimeLeft(bool is_my_player);
std::vector<std::vector<int> > Locations(bool is_my_player);
std::vector<std::vector<int> > Level(bool is_my_player);

bool IsValidRoomNumber(int room_number);
bool HasOtherPlayer();
bool IsDuelRoom();
bool IsMyPlayerLive();
bool IsOtherPlayerLive();


#endif  // CLIENT_SRC_PACMAN_UTIL_H_
