// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string>

#include "pacman_event_handlers.h"
#include "pacman_game_logic.h"
#include "pacman.h"


// private members.
namespace pacman {

// How slow vulnerable ghost move
const int kHowSlow = 3;


void CheckCollision(const PacmanPtr &player) {
  // 캐릭터들의 시작 위치를 가져온다.
  const std::string &start_pos = player->start_points();
  // 캐릭터들의 life(살아있는지 죽어있는지)를 얻어온다.
  std::string str_characters_lives = player->characters_lives();
  // 얻어온 life 변수값을 초기화 한다.
  for (int i = 0; i < (kChrIndexPacman+1); ++i) {
    str_characters_lives[i] = static_cast<char>(1);
  }

  for (int i = 0; i < (kChrIndexGhost3+1); ++i) {
    // 캐릭터들의 위치값을 얻어온다.
    std::string locations = player->locations();

    // i번째 Ghost의 X, Y 인덱스
    const int index_x = (i * 2) + kIndexX;
    const int index_y = (i * 2) + kIndexY;
    // 팩맨의 X, Y 인덱스
    const int pacman_x = (kChrIndexPacman * 2) + kIndexX;
    const int pacman_y = (kChrIndexPacman * 2) + kIndexY;

    // i번째 Ghost의 위치와 팩맨의 위치가 같다면 충돌이다.
    if ((locations[index_x] == locations[pacman_x]) &&
        (locations[index_y] == locations[pacman_y])) {
      if (player->invincible()) {  // 무적상태이면
        // Ghost가 죽어야 한다
        // game point를 계산하여 값을 지정한다.
        // 이 값은 자동으로 클라이언트에 전송된다.
        player->set_game_points(player->game_points() +
            (player->ghosts_in_a_row() * 20));

        // i번째 Ghost의 life를 죽음으로 만든다
        str_characters_lives[i] = static_cast<char>(0);
        // ghosts in a low 값을 2배로 만든다.
        // 이 값은 무적상태에서 여러명의 Ghost를 잡을수록 2배로 증가하며
        // 무적이 풀리면 다시 초기화 된다.
        // 이 값은 자동으로 클라이언트에 전송된다.
        player->set_ghosts_in_a_row(player->ghosts_in_a_row() * 2);

        // 해당 Ghost를 다시 처음위치에서 시작하기 위해 시작위치로
        // 위치값을 지정한다
        locations[index_x] = start_pos[index_x];
        locations[index_y] = start_pos[index_y];
        // 이 값은 클라이언트에 자동으로 전송된다.
        player->set_locations(locations);
      } else {  // 무적상태가 아닌 상태에서 충돌이면 팩맨이 죽은것.
        // 남아있는 life를 차감한다.
        int remain_lives = player->remain_lives();
        --remain_lives;
        // 계산된 life 값을 지정한다.
        // 이 값은 자동으로 클라이언트로 전송된다.
        player->set_remain_lives(remain_lives);

        // 남아있는 life가 없다면 Game Over
        if (remain_lives == -1) {
          // exit message를 지정하며 이 값은 자동으로 클라이언트에 전송된다.
          player->set_exit_message("Game Over");
        }

        // 팩맨의 Life값을 죽음으로 만든다
        str_characters_lives[kChrIndexPacman] = static_cast<char>(0);

        // 모든 캐릭터들의 위치값을 초기값으로 만든다.
        for (int j = 0; j < kChrSize; ++j) {
          const int index_jx = (j * 2) + kIndexX;
          const int index_jy = (j * 2) + kIndexY;
          locations[index_jx] = start_pos[index_jx];
          locations[index_jy] = start_pos[index_jy];
        }
        // 캐릭터들의 위치값을 지정한다.
        // 이 값은 자동으로 클라이언트에 전송된다.
        player->set_locations(locations);

        // 모든 캐릭터들의 방향값을 초기값으로 만든다.
        std::string directions = "";
        directions += static_cast<char>(kDirFront);
        directions += static_cast<char>(kDirStop);
        directions += static_cast<char>(kDirBack);
        directions += static_cast<char>(kDirStop);
        directions += static_cast<char>(kDirStop);
        directions += static_cast<char>(kDirBack);
        directions += static_cast<char>(kDirStop);
        directions += static_cast<char>(kDirFront);
        directions += static_cast<char>(kDirStop);
        directions += static_cast<char>(kDirBack);
        // 초기화된 방향값을 지정한다.
        // 이 값은 자동으로 클라이언트에 전송된다.
        player->set_directions(directions);
     }
    }
  }

  // 캐릭터 들의 life를 지정한다.
  // 이 값은 클라이언트에 자동으로 전송된다.
  player->set_characters_lives(str_characters_lives);
}


void MoveGhosts(const PacmanPtr &player) {
  const int invincible = player->invincible();
  const std::string &cells = player->level();

  if (invincible) {
    player->set_slower_ghosts(player->slower_ghosts()+1);
    if (player->slower_ghosts() > kHowSlow) {
      player->set_slower_ghosts(0);
    }
  }

  if ((!invincible) || player->slower_ghosts() < kHowSlow) {
    // Loop through each ghost
    for (int i = 0; i < (kChrIndexGhost3+1); ++i) {
      std::string locations = player->locations();
      std::string directions = player->directions();

      int checksides[] = { 0, 0, 0, 0, 0, 0 };

      // Switch sides?
      if ((locations[(i * 2) + kIndexX] ==  0) &&
          (directions[(i * 2) + kIndexX] == -1)) {
        // set location
        locations[(i * 2) + kIndexX] = (kLevelWidth-1);
        player->set_locations(locations);
      } else if ((locations[(i * 2) + kIndexX] == (kLevelWidth-1)) &&
                 (directions[(i * 2) + kIndexX] ==  1)) {
        // set location
        locations[(i * 2) + kIndexX] =  0;
        player->set_locations(locations);
      } else if ((locations[(i * 2) + kIndexY] ==  0) &&
                 (directions[(i * 2) + kIndexY] == -1)) {
        // set location
        locations[(i * 2) + kIndexY] = (kLevelHeight-1);
        player->set_locations(locations);
      } else if ((locations[(i * 2) + kIndexY] == (kLevelHeight-1)) &&
                 (directions[(i * 2) + kIndexY] ==  1)) {
        // set location
        locations[(i * 2) + kIndexY] = 0;
        player->set_locations(locations);
      } else {
        // Determine which directions we can go
        for (int j = 0; j < 4; ++j)
          checksides[j] = 0;

        const int xplus_pos = locations[(i * 2) + kIndexX] + 1;
        const int x_pos = xplus_pos - 1;
        const int xminus_pos = x_pos -1;
        const int yplus_pos = locations[(i * 2) + kIndexY] + 1;
        const int y_pos = yplus_pos -1;
        const int yminus_pos = y_pos -1;

        if (cells[(xplus_pos * kLevelHeight) + y_pos] != 1)
          checksides[0] = 1;
        if (cells[(xminus_pos * kLevelHeight) + y_pos] != 1)
          checksides[1] = 1;
        if (cells[(x_pos * kLevelHeight) + yplus_pos] != 1)
          checksides[2] = 1;
        if (cells[(x_pos * kLevelHeight) + yminus_pos] != 1)
          checksides[3] = 1;

        // Don't do 180 unless we have to
        int check = 0;
        for (int j = 0; j < 4; ++j) {
          if (checksides[j] == 1)
            ++check;
        }
        if (check > 1) {
          if (directions[(i * 2) + kIndexX] == kDirFront)
            checksides[1] = 0;
          else if (directions[(i * 2) + kIndexX] == kDirBack)
            checksides[0] = 0;
          else if (directions[(i * 2) + kIndexY] == kDirFront)
            checksides[3] = 0;
          else if (directions[(i * 2) + kIndexY] == kDirBack)
            checksides[2] = 0;
        }

        check = 0;
        int rand_value = 0;

        const int index_x = (i * 2) + kIndexX;
        const int index_y = (i * 2) + kIndexY;
        const int pacman_x = (kChrIndexPacman * 2) + kIndexX;
        const int pacman_y = (kChrIndexPacman * 2) + kIndexY;

        do {
          // Decide direction
          unsigned int rand_internal = player->rand_internal();
          rand_value = rand_r(&rand_internal) / (1625000000 / 4);
          player->set_rand_internal(rand_internal);

          if (checksides[rand_value] == 1) {
            if (rand_value == 0) {
              // set directions
              directions[index_x] = kDirFront;
              directions[index_y] = kDirStop;
              player->set_directions(directions);
            } else if (rand_value == 1) {
              // set directions
              directions[index_x] = kDirBack;
              directions[index_y] = kDirStop;
              player->set_directions(directions);
            } else if (rand_value == 2) {
              // set directions
              directions[index_x] = kDirStop;
              directions[index_y] = kDirFront;
              player->set_directions(directions);
            } else if (rand_value == 3) {
              // set directions
              directions[index_x] = kDirStop;
              directions[index_y] = kDirBack;
              player->set_directions(directions);
            }
          } else {
            if (invincible == 1) {
              // Chase Pacman
              if ((locations[pacman_x] > locations[index_x]) &&
                  (checksides[0] == 1)) {
                directions[index_x] = kDirFront;
                directions[index_y] = kDirStop;
                player->set_directions(directions);
                check = 1;
              } else if ((locations[pacman_x] < locations[index_x]) &&
                         (checksides[1] == 1)) {
                directions[index_x] = kDirBack;
                directions[index_y] = kDirStop;
                player->set_directions(directions);
                check = 1;
              } else if ((locations[pacman_y] > locations[index_y]) &&
                         (checksides[2] == 1)) {
                directions[index_x] = kDirStop;
                directions[index_y] = kDirFront;
                player->set_directions(directions);
                check = 1;
              } else if ((locations[pacman_y] < locations[index_y]) &&
                         (checksides[3] == 1)) {
                directions[index_x] = kDirStop;
                directions[index_y] = kDirBack;
                player->set_directions(directions);
                check = 1;
              }
            } else {
              // Run away from Pacman
              if ((locations[pacman_x] > locations[index_x]) &&
                  (checksides[1] == 1)) {
                directions[index_x] = kDirBack;
                directions[index_y] = kDirStop;
                player->set_directions(directions);
                check = 1;
              } else if ((locations[pacman_x] < locations[index_x]) &&
                         (checksides[0] == 1)) {
                directions[index_x] = kDirFront;
                directions[index_y] = kDirStop;
                player->set_directions(directions);
                check = 1;
              } else if ((locations[pacman_y] > locations[index_y]) &&
                         (checksides[3] == 1)) {
                directions[index_x] = kDirStop;
                directions[index_y] = kDirBack;
                player->set_directions(directions);
                check = 1;
              } else if ((locations[pacman_y] < locations[index_y]) &&
                         (checksides[2] == 1)) {
                directions[index_x] = kDirStop;
                directions[index_y] = kDirFront;
                player->set_directions(directions);
                check = 1;
              }
            }
          }
        } while ((checksides[rand_value] == 0) && (check == 0));

        // Move Ghost
        locations[index_x] += directions[index_x];
        locations[index_y] += directions[index_y];
        player->set_locations(locations);
      }
    }
  }
}


void MovePacman(const PacmanPtr &player) {
  const std::string &directions = player->directions();
  std::string locations = player->locations();
  std::string cells = player->level();

  const int index_x = (kChrIndexPacman * 2) + kIndexX;
  const int index_y = (kChrIndexPacman * 2) + kIndexY;

  // Switch sides?
  if ((locations[index_x] == kLevelStartX) &&
      (directions[index_x] == kDirBack)) {
    locations[index_x] = (kLevelWidth-1);
    player->set_locations(locations);
  } else if ((locations[index_x] == (kLevelWidth-1)) &&
             (directions[index_x] == kDirFront)) {
    locations[index_x] = kLevelStartX;
    player->set_locations(locations);
  } else if ((locations[index_y] == kLevelStartY) &&
             (directions[index_y] == kDirBack)) {
    locations[index_y] = (kLevelHeight-1);
    player->set_locations(locations);
  } else if ((locations[index_y] == (kLevelHeight-1)) &&
             (directions[index_y] == kDirFront)) {
    locations[index_y] = kLevelStartY;
    player->set_locations(locations);
  } else {  // Or
    int pacman_x = locations[index_x] + directions[index_x];
    int pacman_y = locations[index_y] + directions[index_y];
    int index_cell = (pacman_x * kLevelHeight) + pacman_y;

    // If he hit a wall, move back
    if ((cells[index_cell] != kLevelCellWall) &&
        (cells[index_cell] != kLevelCellGhostWall)) {
      locations[index_x] += directions[index_x];
      locations[index_y] += directions[index_y];
      player->set_locations(locations);
    }
  }

  int pacman_x = locations[index_x];
  int pacman_y = locations[index_y];
  int index_cell = (pacman_x * kLevelHeight) + pacman_y;
  // What is he eating?
  switch (cells[index_cell]) {
    case kLevelCellPellet: {   // Pellet
      cells[index_cell] = 0;
      player->set_level(cells);

      int game_points = player->game_points();
      ++game_points;
      player->set_game_points(game_points);

      int left_pellets = player->left_pellets();
      --left_pellets;
      if (left_pellets <= 0) {
        player->set_level_number(player->level_number() + 1);
        player->set_load_status(kNotLoaded);
      } else {
        player->set_left_pellets(left_pellets);
      }
    }
    break;
    case kLevelCellPowerUp: {   // PowerUp
      cells[index_cell] = 0;
      player->set_level(cells);
      player->set_invincible(1);
      if (player->ghosts_in_a_row() == 0) {
        player->set_ghosts_in_a_row(1);
        player->set_invincible_time(time(0));
      }
    }
    break;
  }

  // Is he invincible?
  int time_left = 0;
  if (player->invincible() == 1) {
    time_left = (11 - player->level_number() - time(0) +
                 player->invincible_time());
  }

  // Is invincibility up yet?
  if (time_left < 0) {
    player->set_invincible(0);
    player->set_ghosts_in_a_row(0);
    player->set_time_left(0);
  } else {
    player->set_time_left(time_left);
  }
}

}  // namespace pacman
