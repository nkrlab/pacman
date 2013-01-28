// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

// Some variables that you may want to change


#ifndef CLIENT_SRC_PACMAN_NETWORK_H_
#define CLIENT_SRC_PACMAN_NETWORK_H_

#include "src/pacman_constants.h"


void NetworkInitialize();
void SendMessageLogin(const char *account_id);
void SendMessageLoadLevel(const int level_number);
void SendMessagePacmanMove(const int virtual_key);
void SendMessageLogout();
void SendMessageShowRoomList();
void SendMessageMakeRoom(const char *room_name, const bool is_duel);
void SendMessageGameEndLeaveRoom();
void SendMessageJoinRoom(const int room_number);
void NetworkTerminate();
void HandlingReceivedPacket();


#endif  // CLIENT_SRC_PACMAN_NETWORK_H_
