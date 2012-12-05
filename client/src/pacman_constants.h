// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef CLIENT_SRC_PACMAN_CONSTANTS_H_
#define CLIENT_SRC_PACMAN_CONSTANTS_H_


const int kChrSize = 5;
const int kXYAxis = 2;

const int kLevelStartX = 0;
const int kLevelStartY = 0;
const int kLevelWidth = 29;
const int kLevelHeight = 28;

enum { kLevelCellEmpty = 0,
       kLevelCellWall,
       kLevelCellPellet,
       kLevelCellPowerUp,
       kLevelCellGhostWall,
       kLevelCellGhost0,
       kLevelCellGhost1,
       kLevelCellGhost2,
       kLevelCellGhost3,
       kLevelCellPacman };

enum { kChrIndexGhost0 = 0,
       kChrIndexGhost1,
       kChrIndexGhost2,
       kChrIndexGhost3,
       kChrIndexPacman };

enum { kIndexX = 0,
       kIndexY };

enum { kDirBack = -1,
       kDirStop,
       kDirFront };


#endif  // CLIENT_SRC_PACMAN_CONSTANTS_H_
