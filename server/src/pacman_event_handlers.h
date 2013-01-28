// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef SERVER_SRC_PACMAN_EVENT_HANDLERS_H_
#define SERVER_SRC_PACMAN_EVENT_HANDLERS_H_

#include <funapi/account/account.h>
#include <funapi/common/types.h>

#include "app_messages.pb.h"
#include "pacman_client_messages.pb.h"
#include "pacman_server_messages.pb.h"
#include "pacman_types.h"


namespace pacman {

enum { kNotLoaded = 0, kLoadComplete = 1 };
enum { kSingle = 0, kDuel = 1 };

// the one and only world object.
extern PacmanPtr the_world;

// the current account who sent message.
extern fun::Account::Ptr the_current_account;

// players methods.
PacmanPtr FindPlayer(const string &player_name);
void InsertPlayer(const string &player_name, const PacmanPtr &player);
void ErasePlayer(const string &player_name);

// account message handlers.
void OnLoadLevel(const PacmanPtr &player, const ::LoadLevel &msg);
void OnPacmanMove(const PacmanPtr &player, const ::PacmanMove &msg);
void OnShowRoomList();
void OnMakeRoomGameStart(const PacmanPtr &player,
                         const ::MakeRoomGameStart &msg);
void OnGameEndLeaveRoom(const PacmanPtr &player);
void OnJoinRoom(const PacmanPtr &player, const ::JoinRoom &msg);

// game tick
void GameTick();

}  // namespace pacman

#endif  // SERVER_SRC_PACMAN_EVENT_HANDLERS_H_
