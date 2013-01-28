// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

// Some variables that you may want to change


#ifndef CLIENT_SRC_PACMAN_H_
#define CLIENT_SRC_PACMAN_H_

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <map>
#include <string>
#include <vector>

#include "src/util/archive.h"


class Pacman;
typedef boost::uuids::uuid Uuid;
typedef boost::shared_ptr<Pacman> PacmanPtr;
typedef std::vector<PacmanPtr> PacmanPtrVector;
typedef std::map<std::string, PacmanPtr> PacmanPtrMap;


class Pacman {
 public:
  Pacman();
  ~Pacman();

  static const PacmanPtr kNull;

  static PacmanPtr CreateFromSerialized(const fun::Archive &archive);

  Uuid GetUuid() { return uuid_; }
  void SetUuid(Uuid value) { uuid_ = value; }

  int GamePoints() { return game_points_; }
  void SetGamePoints(int value) { game_points_ = value; }

  int GhostsInARow() { return ghosts_in_a_row_; }
  void SetGhostsInARow(int value) { ghosts_in_a_row_ = value; }

  int Invincible() { return invincible_; }
  void SetInvincible(int value) { invincible_ = value; }

  int LevelNumber() { return level_number_; }
  void SetLevelNumber(int value) { level_number_ = value; }

  int RemainLives() { return remain_lives_; }
  void SetRemainLives(int value) { remain_lives_ = value; }

  int TimeLeft() { return time_left_; }
  void SetTimeLeft(int value) { time_left_ = value; }

  std::string Name() { return name_; }
  void SetName(std::string value) { name_ = value; }

  std::string Locations() { return locations_; }
  void SetLocations(std::string value) { locations_ = value; }

  std::string Level() { return level_; }
  void SetLevel(std::string value) { level_ = value; }

  std::string CharactersLives() { return characters_lives_; }
  void SetCharactersLives(std::string value) { characters_lives_ = value; }

  std::string ExitMessage() { return exit_message_; }
  void SetExitMessage(std::string value) { exit_message_ = value; }

  PacmanPtrVector& GameRooms() { return game_rooms_; }
  void SetGameRooms(PacmanPtrVector value) { game_rooms_ = value; }

  PacmanPtrMap& Players() { return players_; }
  void SetPlayers(PacmanPtrMap value) { players_ = value; }


 private:
  // uuid
  Uuid uuid_;

  // int attributes
  int game_points_;
  int ghosts_in_a_row_;
  int invincible_;
  int level_number_;
  int remain_lives_;
  int time_left_;

  // string attributes
  std::string name_;
  std::string locations_;
  std::string level_;
  std::string characters_lives_;
  std::string exit_message_;

  // object vector attribute
  PacmanPtrVector game_rooms_;

  // object map attribute
  PacmanPtrMap players_;
};

#endif  // CLIENT_SRC_PACMAN_H_
