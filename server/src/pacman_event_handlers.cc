// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include "pacman_event_handlers.h"

#include <boost/foreach.hpp>
#include <funapi/api/clock.h>
#include <funapi/object/object.h>
#include <funapi/common/serialization/bson_archive.h>
#include <funapi/system/logging.h>
#include <string>

#include "pacman.h"
#include "pacman_game_logic.h"
#include "pacman_types.h"


namespace pacman {

enum { kLoadComplete = 1 };

PacmanPtr the_world;
fun::Account::Ptr the_current_account;


PacmanPtr FindPlayer(const string &player_name) {
  return the_world->players().Find(player_name);
}


void InsertPlayer(const PacmanPtr &player) {
  PacmanPtrMap players = the_world->players();
  players.Insert(player->name(), player);
  the_world->set_players(players);
}


void ErasePlayer(const string &player_name) {
  PacmanPtrMap players = the_world->players();
  players.erase(player_name);
  the_world->set_players(players);
}


void OnLoadLevel(const PacmanPtr &/*player*/, const ::LoadLevel &msg) {
  // Game Login을 초기화 한다.
  InitializeGameLogic();
  const int &level_number = msg.level_number();

  // 여러가지 attributes의 초기값들을 지정한다.
  // 이 값들은 자동으로 클라이언트에 전송된다.
  the_world->set_level_number(level_number);
  // 맵이 1레벨일때만 remain life 값 지정.
  if (level_number == 1)
    the_world->set_remain_lives(3);
  the_world->set_game_points(0);
  the_world->set_time_left(0);
  the_world->set_ghosts_in_a_row(0);

  // 캐릭터들의 초기 방향값을 지정한다.
  std::string str_directions = "";
  str_directions += static_cast<char>(kDirFront);
  str_directions += static_cast<char>(kDirStop);
  str_directions += static_cast<char>(kDirBack);
  str_directions += static_cast<char>(kDirStop);
  str_directions += static_cast<char>(kDirStop);
  str_directions += static_cast<char>(kDirBack);
  str_directions += static_cast<char>(kDirStop);
  str_directions += static_cast<char>(kDirFront);
  str_directions += static_cast<char>(kDirStop);
  str_directions += static_cast<char>(kDirBack);
  // 이 값은 클라이언트에 자동으로 전송된다.
  the_world->set_directions(str_directions);

  // file open
  char level_file[50] = "../src/resources/level0_.dat";
  level_file[strlen(level_file) - 5] = level_number + '0';

  // Open file
  FILE *fin = fopen(level_file, "r");
  // Make sure it didn't fail
  if (!fin) {
    assert(0);
    return;
  }

  // Open file and load the level into the array
  // 캐릭터들의 초기 위치값을 저장할 변수를 만들고 공간을 할당한다.
  std::string str_locations = "";
  str_locations.resize(kChrSize * kXYAxis, ' ');

  // 남아있는 pellet(*모양의 먹이)를 세기 위한 변수를 선언하고 초기화한다.
  int left_pellets = 0;

  // 레벨의 각 cell을 저장할 변수를 string으로 선언한다.
  std::string level_cells = "";
  for (int i = 0; i < kLevelWidth; ++i) {
    for (int j = 0; j < kLevelHeight; ++j) {
      int level_cell;
      if (fscanf(fin, "%d", &level_cell) == EOF) {
        assert(0);
        return;
      }

      // cell이 pellet이면 남아있는 pellet의 갯수를 증가시킨다.
      if (level_cell == kLevelCellPellet) {
        ++left_pellets;
      }

      // cell이 캐릭터이면 위치를 저장하고 해당 cell을 비어있는 상태로 만든다.
      if (level_cell == kLevelCellGhost0) {
        str_locations[kIndexX] = i;
        str_locations[kIndexY] = j;
        level_cells += static_cast<char>(kLevelCellEmpty);
      } else if (level_cell == kLevelCellGhost1) {
        str_locations[kChrIndexGhost1*2 + kIndexX] = i;
        str_locations[kChrIndexGhost1*2 + kIndexY] = j;
        level_cells += static_cast<char>(kLevelCellEmpty);
      } else if (level_cell == kLevelCellGhost2) {
        str_locations[kChrIndexGhost2*2 + kIndexX] = i;
        str_locations[kChrIndexGhost2*2 + kIndexY] = j;
        level_cells += static_cast<char>(kLevelCellEmpty);
      } else if (level_cell == kLevelCellGhost3) {
        str_locations[kChrIndexGhost3*2 + kIndexX] = i;
        str_locations[kChrIndexGhost3*2 + kIndexY] = j;
        level_cells += static_cast<char>(kLevelCellEmpty);
      } else if (level_cell == kLevelCellPacman) {
        str_locations[kChrIndexPacman*2 + kIndexX] = i;
        str_locations[kChrIndexPacman*2 + kIndexY] = j;
        level_cells += static_cast<char>(kLevelCellEmpty);
      } else {
        level_cells += static_cast<char>(level_cell);
      }
    }
  }

  // 파일을 닫는다.
  fclose(fin);

  // 계산된 pellet의 갯수를 저장한다.
  // 이 값은 클라이언트에 자동으로 전송된다.
  the_world->set_left_pellets(left_pellets);

  // 캐릭터들의 위치정보를 저장한다.
  // 이 값은 클라이언트에 자동으로 전송된다.
  the_world->set_locations(str_locations);

  // 레벨(맵 데이터)를 저장한다.
  // 이 값은 클라이언트데 자동으로 전송된다.
  the_world->set_level(level_cells);

  // 캐릭터들의 시작지점을 저장한다.
  // 이 값은 클라이언트에 자동으로 전송된다.
  the_world->set_start_points(str_locations);

  // 팩맨의 무적 Flag를 초기화 한다.
  // 이 값은 클라이언트에 자동으로 전송된다.
  the_world->set_invincible(0);

  // 캐릭터들의 life(살아있는지 죽어있는지)상태를 초기화 한다.
  std::string str_characters_lives = "";
  str_characters_lives += static_cast<char>(1);
  str_characters_lives += static_cast<char>(1);
  str_characters_lives += static_cast<char>(1);
  str_characters_lives += static_cast<char>(1);
  str_characters_lives += static_cast<char>(1);
  // 캐릭터들의 life상태를 저장한다.
  // 이 값은 클라이언트데 자동으로 전송된다.
  the_world->set_characters_lives(str_characters_lives);

  // load complete
  the_world->set_load_complete(kLoadComplete);
}


void OnPacmanMove(const PacmanPtr &/*player*/, const ::PacmanMove &msg) {
  // 클라이언트에서 받은 메시지에서 팩맨의 가고자 하는 방향값을 추출한다.
  const int &pacman_direction = msg.pacman_direction();

  // 월드에 저장된 레벨의 cell 값들을 읽어온다.
  const std::string &level_cells = the_world->level();
  // 월드에 저장된 캐릭터들의 위치값을 읽어온다.
  const std::string &locations = the_world->locations();
  // 월드에 저장된 캐릭터들의 방향값을 읽어온다.
  std::string directions = the_world->directions();

  // 캐릭터들의 위치값들과 방향값들에 적용하기 위해 팩맨의 Index를 구한다.
  int x_index = kChrIndexPacman * 2 + kIndexX;
  int y_index = kChrIndexPacman * 2 + kIndexY;
  // 레벨에서 팩맨의 x좌표와 y좌표를 구한다.
  int x_pos = locations[x_index];
  int y_pos = locations[y_index];

  switch (pacman_direction) {
  case 0403: {  // key code left
      // 팩맨의 위치에서 왼쪽의 cell을 얻어온다.
      int cell = level_cells[(x_pos - 1) * kLevelHeight + y_pos];
      // 벽이 아니고 GhostWall(Npc만 통과가능한 벽)이 아니면
      if ((cell != kLevelCellWall) && (cell != kLevelCellGhostWall)) {
        directions[x_index] = kDirBack;  // x좌표를 -1 이동(왼쪽)
        directions[y_index] = kDirStop;  // y좌표는 이동하지 않는다.
      }
    }
    break;
  case 0402: {  // key code right
      // 팩맨의 위치에서 오른쪽 cell을 얻어온다.
      int cell = level_cells[(x_pos + 1) * kLevelHeight + y_pos];
      // 벽이 아니고 GhostWall(Npc만 통과가능한 벽)이 아니면
      if ((cell != kLevelCellWall) && (cell != kLevelCellGhostWall)) {
        directions[x_index] = kDirFront;  // x좌표를 +1 이동(오른쪽)
        directions[y_index] = kDirStop;   // y좌표는 이동하지 않는다.
      }
    }
    break;
  case 0404: {  // key code down
      // 팩맨의 위치에서 아래쪽 cell을 얻어온다.
      int cell = level_cells[x_pos * kLevelHeight + y_pos - 1];
      // 벽이 아니고 GhostWall(Npc만 통과가능한 벽)이 아니면
      if ((cell != kLevelCellWall) && (cell != kLevelCellGhostWall)) {
        directions[x_index] = kDirStop;  // x좌표는 이동하지 않는다.
        directions[y_index] = kDirBack;  // y좌표를 -1 이동(아래쪽)
      }
    }
    break;
  case 0405: {  // key code up
      // 팩맨의 위치에서 아래쪽 cell을 얻어온다.
      int cell = level_cells[x_pos*kLevelHeight + y_pos+1];
      // 벽이 아니고 GhostWall(Npc만 통과가능한 벽)이 아니면
      if ((cell != kLevelCellWall) && (cell != kLevelCellGhostWall)) {
        directions[x_index] = kDirStop;   // x좌표는 이동하지 않는다.
        directions[y_index] = kDirFront;  // y좌표를 +1 이동(위쪽)
      }
    }
    break;
  }

  // 캐릭터들의 방향값을 저장한다(팩맨의 방향값만 변경되었다)
  // 이 값은 클라이언트데 자동으로 전송된다.
  the_world->set_directions(directions);

  // 방향값에 따라 팩맨을 이동시킨다.
  MovePacman();
  // 충돌 체크를 한다.
  CheckCollision();
}


void GameTick() {
  if (the_world->load_complete()) {
    // 클라이언트에서 요청이 오면
    // Ghost 들을 이동시킨다.
    MoveGhosts();
    // 충돌 체크를 한다.
    CheckCollision();
  }
}

}  // namespace pacman
