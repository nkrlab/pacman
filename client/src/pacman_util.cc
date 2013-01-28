// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include "src/account_messages.pb.h"
#include "src/pacman.h"
#include "src/pacman_app.h"
#include "src/pacman_constants.h"
#include "src/pacman_render.h"
#include "src/pacman_util.h"
#include "src/util/json_archive.h"
#include "src/util/string_convert.h"


typedef ::AttributeUpdatesMessage::AttributeUpdateMessage AttributeUpdate;

const PacmanPtr Pacman::kNull;


namespace {
std::string account_id;
PacmanPtr the_world;


PacmanPtr FindPacmanObject(PacmanPtr object, const Uuid &uuid) {
  if (object->GetUuid() == uuid)
    return object;

  size_t size = object->GameRooms().size();
  for (size_t i = 0; i < size; ++i) {
    PacmanPtr found = FindPacmanObject(object->GameRooms()[i], uuid);
    if (found != Pacman::kNull)
      return found;
  }

  PacmanPtrMap::iterator it;
  for (it = object->Players().begin(); it != object->Players().end(); ++it) {
    PacmanPtr found = FindPacmanObject(it->second, uuid);
    if (found != Pacman::kNull)
      return found;
  }
  return Pacman::kNull;
}


PacmanPtr FindMyPlayer() {
  size_t size = the_world->GameRooms().size();
  for (size_t i = 0; i < size; ++i) {
    PacmanPtr room = the_world->GameRooms()[i];
    PacmanPtrMap::iterator it;
    for (it = room->Players().begin(); it != room->Players().end(); ++it) {
      if (it->first.compare(account_id) == 0)
        return it->second;
    }
  }
  return Pacman::kNull;
}


PacmanPtr FindMyRoom() {
  size_t size = the_world->GameRooms().size();
  for (size_t i = 0; i < size; ++i) {
    PacmanPtr room = the_world->GameRooms()[i];
    PacmanPtrMap::iterator it;
    for (it = room->Players().begin(); it != room->Players().end(); ++it) {
      if (it->first.compare(account_id) == 0)
        return room;
    }
  }
  return Pacman::kNull;
}


PacmanPtr FindOtherPlayer() {
  PacmanPtr room = FindMyRoom();
  if (not room)
    return Pacman::kNull;

  PacmanPtrMap::iterator it;
  for (it = room->Players().begin(); it != room->Players().end(); ++it) {
    if (it->first.compare(account_id) != 0)
      return it->second;
  }
  return Pacman::kNull;
}


bool IsPlayerLive(bool is_my_player) {
  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (not pacman)
    return false;

  if (pacman->RemainLives() > -1)
    return true;
  return false;
}


Uuid GetPacmanUuid(const AttributeUpdate &kAttributeMsg) {
  const ::std::string& uuid_string = kAttributeMsg.object_uuid();
  Uuid uuid;
  ::memcpy(uuid.data, uuid_string.data(), Uuid::static_size());
  return uuid;
}


int GetAttributeInt(const AttributeUpdate &kAttributeMsg) {
  json_spirit::Value value;
  json_spirit::read(kAttributeMsg.new_json(), value);
  return value.get_int();
}


std::string GetAttributeString(const AttributeUpdate &kAttributeMsg) {
  json_spirit::Value value;
  json_spirit::read(kAttributeMsg.new_json(), value);
  return value.get_str();
}


PacmanPtrVector GetAttributePacmanPtrVector(
    const AttributeUpdate &kAttributeMsg) {
  json_spirit::mValue value;
  json_spirit::read(kAttributeMsg.new_json(), value);
  json_spirit::mArray array = value.get_array();

  fun::ArchivePtrArray archive;
  fun::JsonArchive::Reader::FromJson(array, &archive);
  PacmanPtrVector obj_vector;
  BOOST_FOREACH(const fun::ArchivePtr &element, archive) {
    PacmanPtr object_ptr = Pacman::CreateFromSerialized(*element);
    obj_vector.push_back(object_ptr);
  }
  return obj_vector;
}


PacmanPtrMap GetAttributePacmanPtrMap(const AttributeUpdate &kAttributeMsg) {
  json_spirit::mValue value;
  json_spirit::read(kAttributeMsg.new_json(), value);
  json_spirit::mObject object = value.get_obj();

  fun::ArchivePtrObject archive;
  fun::JsonArchive::Reader::FromJson(object, &archive);
  PacmanPtrMap obj_map;
  BOOST_FOREACH(const fun::ArchivePtrObject::value_type &element, archive) {
    PacmanPtr object_ptr = Pacman::CreateFromSerialized(*(element.second));
    obj_map.insert(make_pair(element.first, object_ptr));
  }
  return obj_map;
}


void SetGamePoints(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman) {
    return;
  }
  pacman->SetGamePoints(value);
}


void SetGhostsInARow(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetGhostsInARow(value);
}


void SetInvincible(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetInvincible(value);
}


void SetLevelNumber(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetLevelNumber(value);
}


void SetRemainLives(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetRemainLives(value);
}


void SetTimeLeft(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetTimeLeft(value);
}


void SetLocations(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetLocations(value);
}


void SetLevel(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetLevel(value);
}


void SetCharactersLives(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetCharactersLives(value);
  value = Decode(value);

  bool is_my_player = false;
  PacmanPtr player = FindMyPlayer();
  if (player) {
    is_my_player = true;
  } else {
    player = FindOtherPlayer();
    if (player) {
      is_my_player = false;
    } else {
      return;
    }
  }

  if (pacman->GetUuid() == player->GetUuid()) {
    // 직접 render의 함수를 call
    std::vector<std::vector<int> > locations = Locations(true);
    int ghosts_in_a_row = GhostsInARow(true);

    for (size_t i = 0; i < value.size(); ++i) {
      int data = static_cast<int>(value[i]);
      if (data == 0) {
        if (i < kChrIndexPacman) {
          ShowGhostDie(locations[i][kIndexX], locations[i][kIndexY],
                       ghosts_in_a_row/2, is_my_player);
        } else {
          ShowPacmanDie(locations[i][kIndexX], locations[i][kIndexY],
                        is_my_player);
        }
        SetNeedSleep();
      }
    }
  }
}


void SetExitMessage(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetExitMessage(value);

  // 나의 player의 message이면
  PacmanPtr player = FindMyPlayer();
  if (not player)
    return;

  if (pacman->GetUuid() == player->GetUuid())
    OnChangeExitMessage(value);
}


void SetGameRooms(PacmanPtrVector value) {
  the_world->SetGameRooms(value);
}


void SetPlayers(Uuid uuid, PacmanPtrMap value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (not pacman)
    return;
  pacman->SetPlayers(value);

  // 나의 room의 message이면
  PacmanPtr room = FindMyRoom();
  if (pacman != room)
    return;

  if (room->Players().size() > 1)
    ReceivedOtherPlayerJoin();
}


void AttributeParseRecursively(const AttributeUpdate &kAttributeMsg) {
  Uuid uuid = GetPacmanUuid(kAttributeMsg);
  std::string name = kAttributeMsg.attribute_name();

  if (name.compare("game_points") == 0) {
    int value = GetAttributeInt(kAttributeMsg);
    SetGamePoints(uuid, value);
  } else if (name.compare("ghosts_in_a_row") == 0) {
    int value = GetAttributeInt(kAttributeMsg);
    SetGhostsInARow(uuid, value);
  } else if (name.compare("invincible") == 0) {
    int value = GetAttributeInt(kAttributeMsg);
    SetInvincible(uuid, value);
  } else if (name.compare("level_number") == 0) {
    int value = GetAttributeInt(kAttributeMsg);
    SetLevelNumber(uuid, value);
    ReceivedLevelNumber(value);
  } else if (name.compare("remain_lives") == 0) {
    int value = GetAttributeInt(kAttributeMsg);
    SetRemainLives(uuid, value);
  } else if (name.compare("time_left") == 0) {
    int value = GetAttributeInt(kAttributeMsg);
    SetTimeLeft(uuid, value);
  } else if (name.compare("locations") == 0) {
    std::string value = GetAttributeString(kAttributeMsg);
    SetLocations(uuid, value);
  } else if (name.compare("level") == 0) {
    std::string value = GetAttributeString(kAttributeMsg);
    SetLevel(uuid, value);
  } else if (name.compare("characters_lives") == 0) {
    std::string value = GetAttributeString(kAttributeMsg);
    SetCharactersLives(uuid, value);
  } else if (name.compare("exit_message") == 0) {
    std::string value = GetAttributeString(kAttributeMsg);
    SetExitMessage(uuid, value);
  } else if (name.compare("game_rooms") == 0) {
    // set world uuid
    the_world->SetUuid(uuid);
    // set attribute
    PacmanPtrVector vector = GetAttributePacmanPtrVector(kAttributeMsg);
    SetGameRooms(vector);
    // received room list
    ReceivedRoomList();
  } else if (name.compare("players") == 0) {
    PacmanPtrMap map = GetAttributePacmanPtrMap(kAttributeMsg);
    SetPlayers(uuid, map);
  }
}

}  // anonymous namespace end


void SetAccountId(const std::string &name) {
  account_id = name;
}


std::vector<RoomInfo> GetRoomList() {
  std::vector<RoomInfo> result;
  size_t size = the_world->GameRooms().size();

  for (size_t i = 0; i < size; ++i) {
    PacmanPtr room = the_world->GameRooms()[i];

    boost::format fmter("%d : %s");
    fmter % i;
    fmter % room->Name();

    RoomInfo room_info;
    room_info.room_name_ = fmter.str();

    if (room->Duel() == kDuel) {
      room_info.room_name_ += " (Duel)";
      room_info.duel_ = kDuel;
    } else {
      room_info.duel_ = kSingle;
    }

    result.push_back(room_info);
  }

  return result;
}


void InitializeWorld() {
  the_world = PacmanPtr(new Pacman);
}


void UpdateFromSerializedBuffer(const std::string &buffer) {
  ServerAccountMessage sa_msg;
  sa_msg.ParseFromString(buffer);

  ServerAccountMessage_Type account_type = sa_msg.type();
  if (account_type == ServerAccountMessage::kAttributeUpdatesMessage) {
    const AttributeUpdatesMessage &updates = sa_msg.attribute_updates();
    const int updates_size = updates.attribute_update_size();

    for (int i = 0; i < updates_size; ++i) {
      const AttributeUpdate &kAttributeMsg = updates.attribute_update(i);
      AttributeParseRecursively(kAttributeMsg);
    }
  } else if (account_type == ServerAccountMessage::kAccountLoginResponse) {
    // notify login response
    ReceivedLoginResponse();
  }
}


int GamePoints(bool is_my_player) {
  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (pacman)
    return pacman->GamePoints();
  return 0;
}


int GhostsInARow(bool is_my_player) {
  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (pacman)
    return pacman->GhostsInARow();
  return 0;
}


bool Invincible(bool is_my_player) {
  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (pacman) {
    if (pacman->Invincible() == 1)
      return true;
  }
  return false;
}


int LevelNumber(bool is_my_player) {
  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (pacman)
    return pacman->LevelNumber();
  return 0;
}


int RemainLives(bool is_my_player) {
  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (pacman)
    return pacman->RemainLives();
  return 0;
}


int TimeLeft(bool is_my_player) {
  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (pacman)
    return pacman->TimeLeft();
  return 0;
}


std::vector<std::vector<int> > Locations(bool is_my_player) {
  std::vector<std::vector<int> > result;

  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (not pacman)
    return result;

  std::string locations = Decode(pacman->Locations());
  for (int i = 0; i < kChrSize; ++i) {
    std::vector<int> positions;
    for (int j = 0; j < kXYAxis; ++j) {
      int data = static_cast<int>(locations[(i * kXYAxis) + j]);
      positions.push_back(data);
    }
    result.push_back(positions);
  }
  return result;
}


std::vector<std::vector<int> > Level(bool is_my_player) {
  std::vector<std::vector<int> > result;

  PacmanPtr pacman = (is_my_player) ? FindMyPlayer() : FindOtherPlayer();
  if (not pacman)
    return result;

  std::string level = Decode(pacman->Level());
  for (int i = 0; i < kLevelWidth; ++i) {
    std::vector<int> column_cell;
    for (int j = 0; j < kLevelHeight; ++j) {
      int data = static_cast<int>(level[(i * kLevelHeight) + j]);
      column_cell.push_back(data);
    }
    result.push_back(column_cell);
  }
  return result;
}


bool IsValidRoomNumber(int room_number) {
  if (room_number < 0)
    return false;

  size_t index = static_cast<size_t>(room_number);
  size_t size = the_world->GameRooms().size();
  if (index >= size)
    return false;

  PacmanPtr pacman = the_world->GameRooms().at(room_number);
  if (not pacman)
    return false;

  if (not pacman->Duel())
    return false;

  if (pacman->Players().size() > 1)
    return false;

  return true;
}


bool HasOtherPlayer() {
  bool bRet = false;
  if (FindOtherPlayer())
    bRet = true;
  return bRet;
}


bool IsDuelRoom() {
  PacmanPtr room = FindMyRoom();
  if (not room)
    return false;

  if (room->Duel() == kDuel)
    return true;
  return false;
}


bool IsMyPlayerLive() {
  return IsPlayerLive(true);
}


bool IsOtherPlayerLive() {
  return IsPlayerLive(false);
}
