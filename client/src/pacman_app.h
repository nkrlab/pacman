// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

// Some variables that you may want to change


#ifndef CLIENT_SRC_PACMAN_APP_H_
#define CLIENT_SRC_PACMAN_APP_H_

#include <map>
#include <string>
#include <vector>

#include "src/pacman_constants.h"


void DoExitProgram(const char *kMessage);
void OnChangeExitMessage(const std::string &exit_message);
void SetNeedSleep();
void ReceivedRoomList();
void ReceivedLevelNumber(int level_number);
void ReceivedLoginResponse();
void ReceivedOtherPlayerJoin();


#endif  // CLIENT_SRC_PACMAN_APP_H_
