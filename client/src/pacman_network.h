// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

// Some variables that you may want to change


#ifndef CLIENT_SRC_PACMAN_NETWORK_H_
#define CLIENT_SRC_PACMAN_NETWORK_H_

#include "src/pacman_constants.h"

enum kPacketType { kLogin = 1, kLoadLevel, kPacmanMove, kRequestTick, kLogout,
                   kShowRoomList, kMakeRoomGameStart, kGameEndLeaveRoom };
enum { kNoUse = -1 };


void NetworkInitialize();
void SendMessage(const kPacketType kType, const int value);
void SendMessage(const kPacketType kType, const char *string1,
                 const char *string2);
void NetworkTerminate();
void HandlingReceivedPacket();


#endif  // CLIENT_SRC_PACMAN_NETWORK_H_
