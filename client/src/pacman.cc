// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <boost/foreach.hpp>

#include "src/pacman.h"
#include "src/util/boost_util.h"
#include "src/util/json_util.h"
#include "src/util/json_archive.h"


Pacman::Pacman() {
  uuid_ = boost::uuids::nil_uuid();

  // int attributes
  game_points_ = 0;
  ghosts_in_a_row_ = 0;
  invincible_ = 0;
  level_number_ = 0;
  remain_lives_ = 3;
  time_left_ = 0;
  duel_ = kDuel;

  // string attributes
  name_ = "";
  locations_ = "";
  level_ = "";
  characters_lives_ = "";
  exit_message_ = "";
}


Pacman::~Pacman() {
}


PacmanPtr Pacman::CreateFromSerialized(const fun::Archive &archive) {
  PacmanPtr obj_ptr(new Pacman);
  fun::Archive::ReaderPtr reader = archive.CreateReader();

  fun::UuidPtr uuid_value = reader->ReadUuid("_id");
  if (uuid_value != fun::UuidPtr())
    obj_ptr->SetUuid(*uuid_value);

  fun::IntegerPtr int_value = reader->ReadInteger("game_points");
  if (int_value != fun::IntegerPtr())
    obj_ptr->SetGamePoints(*int_value);

  int_value = reader->ReadInteger("ghosts_in_a_row");
  if (int_value != fun::IntegerPtr())
    obj_ptr->SetGhostsInARow(*int_value);

  int_value = reader->ReadInteger("invincible");
  if (int_value != fun::IntegerPtr())
    obj_ptr->SetInvincible(*int_value);

  int_value = reader->ReadInteger("level_number");
  if (int_value != fun::IntegerPtr())
    obj_ptr->SetLevelNumber(*int_value);

  int_value = reader->ReadInteger("remain_lives");
  if (int_value != fun::IntegerPtr())
    obj_ptr->SetRemainLives(*int_value);

  int_value = reader->ReadInteger("time_left");
  if (int_value != fun::IntegerPtr())
    obj_ptr->SetTimeLeft(*int_value);

  int_value = reader->ReadInteger("duel");
  if (int_value != fun::IntegerPtr())
    obj_ptr->SetDuel(*int_value);

  fun::StringPtr str_value = reader->ReadString("name");
  if (str_value != fun::StringPtr())
    obj_ptr->SetName(*str_value);

  str_value = reader->ReadString("locations");
  if (str_value != fun::StringPtr())
    obj_ptr->SetLocations(*str_value);

  str_value = reader->ReadString("level");
  if (str_value != fun::StringPtr())
    obj_ptr->SetLevel(*str_value);

  str_value = reader->ReadString("characters_lives");
  if (str_value != fun::StringPtr())
    obj_ptr->SetCharactersLives(*str_value);

  str_value = reader->ReadString("exit_message");
  if (str_value != fun::StringPtr())
    obj_ptr->SetExitMessage(*str_value);

  fun::ArchivePtrArrayPtr game_rooms_archive =
      reader->ReadArchivePtrArray("game_rooms");
  PacmanPtrVector obj_vector;
  if (game_rooms_archive != fun::ArchivePtrArrayPtr()) {
    BOOST_FOREACH(const fun::ArchivePtr &element, *game_rooms_archive) {
      PacmanPtr object_ptr = Pacman::CreateFromSerialized(*element);
      obj_vector.push_back(object_ptr);
    }
  }
  obj_ptr->SetGameRooms(obj_vector);

  fun::ArchivePtrObjectPtr players_archive =
    reader->ReadArchivePtrObject("players");
  PacmanPtrMap obj_map;
  if (players_archive != fun::ArchivePtrObjectPtr()) {
    BOOST_FOREACH(const fun::ArchivePtrObject::value_type &element,
                *players_archive) {
      PacmanPtr object_ptr = Pacman::CreateFromSerialized(*(element.second));
      obj_map.insert(make_pair(element.first, object_ptr));
    }
  }
  obj_ptr->SetPlayers(obj_map);

  return obj_ptr;
}
