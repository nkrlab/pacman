// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef SERVER_SRC_PACMAN_GAME_LOGIC_H_
#define SERVER_SRC_PACMAN_GAME_LOGIC_H_

#include "pacman_types.h"


namespace pacman {

// 캐릭터들의 총 갯수
const int kChrSize = 5;
// 좌표축의 갯수(x, y축 2개)
const int kXYAxis = 2;
// 레벨의 시작 좌표
const int kLevelStartX = 0;
const int kLevelStartY = 0;
// 레벨의 width, height
const int kLevelWidth = 29;
const int kLevelHeight = 28;

// 레벨데이터에서 값(맵 데이터를 파일에서 읽으면 이 값들이 있다)
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
// 캐릭터 index
enum { kChrIndexGhost0 = 0,
       kChrIndexGhost1,
       kChrIndexGhost2,
       kChrIndexGhost3,
       kChrIndexPacman };
// 좌표축 index
enum { kIndexX = 0,
       kIndexY };
// 방향값 index. 이 값만큼 해당 축에 따라 이동한다.
enum { kDirBack = -1,
       kDirStop,
       kDirFront };

// Attribute에 포함되지 않는 Game Logic 관련 변수들을 초기화한다.
void InitializeGameLogic();
// 충돌체크를 한다.
void CheckCollision();
// Ghost들을 움직인다.
void MoveGhosts();
// 팩맨을 움직인다.
void MovePacman();

}  // namespace pacman

#endif  // SERVER_SRC_PACMAN_GAME_LOGIC_H_
