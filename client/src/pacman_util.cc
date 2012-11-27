// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <boost/foreach.hpp>

#include "src/account_messages.pb.h"
#include "src/pacman.h"
#include "src/pacman_app.h"
#include "src/pacman_constants.h"
#include "src/pacman_render.h"
#include "src/pacman_util.h"
#include "src/util/json_archive.h"


typedef ::AttributeUpdatesMessage::AttributeUpdateMessage AttributeUpdate;


namespace {
std::string account_id;
PacmanPtr the_world;


PacmanPtr FindPacmanObject(PacmanPtr object, const Uuid &uuid) {
  if (object->GetUuid() == uuid)
    return object;

  size_t size = object->GameRooms().size();
  for (size_t i = 0; i < size; ++i) {
    PacmanPtr found = FindPacmanObject(object->GameRooms()[i], uuid);
    if (found != PacmanPtr())
      return found;
  }

  PacmanPtrMap::iterator it;
  for (it = object->Players().begin(); it != object->Players().end(); ++it) {
    PacmanPtr found = FindPacmanObject(it->second, uuid);
    if (found != PacmanPtr())
      return found;
  }
  return PacmanPtr();
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
  return PacmanPtr();
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
  if (pacman == PacmanPtr()) {
    return;
  }
  pacman->SetGamePoints(value);
}


void SetGhostsInARow(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetGhostsInARow(value);
}


void SetInvincible(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetInvincible(value);
}


void SetLevelNumber(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetLevelNumber(value);
}


void SetRemainLives(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetRemainLives(value);
}


void SetTimeLeft(Uuid uuid, int value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetTimeLeft(value);
}


void SetLocations(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetLocations(value);
}


void SetLevel(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetLevel(value);
}


void SetCharactersLives(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetCharactersLives(value);

  // 나의 player의 message이면
  PacmanPtr player = FindMyPlayer();
  if (player == PacmanPtr())
    return;

  if (pacman->GetUuid() == player->GetUuid()) {
    // 직접 render의 함수를 call
    std::vector<std::vector<int> > locations = Locations();
    int ghosts_in_a_row = GhostsInARow();

    for (size_t i = 0; i < value.size(); ++i) {
      int data = static_cast<int>(value[i]);
      if (data == 0) {
        if (i < kChrIndexPacman) {
          ShowGhostDie(locations[i][kIndexX], locations[i][kIndexY],
                       ghosts_in_a_row/2);
        } else {
          ShowPacmanDie(locations[i][kIndexX], locations[i][kIndexY]);
        }
        SetNeedSleep();
      }
    }
  }
}


void SetExitMessage(Uuid uuid, std::string value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetExitMessage(value);

  // 나의 player의 message이면
  PacmanPtr player = FindMyPlayer();
  if (player == PacmanPtr())
    return;

  if (pacman->GetUuid() == player->GetUuid())
    OnChangeExitMessage(value);
}


void SetGameRooms(PacmanPtrVector value) {
  the_world->SetGameRooms(value);
}


void SetPlayers(Uuid uuid, PacmanPtrMap value) {
  PacmanPtr pacman = FindPacmanObject(the_world, uuid);
  if (pacman == PacmanPtr())
    return;
  pacman->SetPlayers(value);
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


std::vector<std::string> GetRoomList() {
  std::vector<std::string> result;
  size_t size = the_world->GameRooms().size();
  int index = 1;
  for (size_t i = 0; i < size; ++i) {
    PacmanPtr room = the_world->GameRooms()[i];

    PacmanPtrMap::iterator it;
    for (it = room->Players().begin(); it != room->Players().end(); ++it) {
      const int kBufferSize = 128;
      char buffer[kBufferSize];
      snprintf(buffer, kBufferSize, "%d", index);
      ++index;

      std::string room_name;
      room_name = buffer;
      room_name += " :  ";
      room_name += room->Name();
      room_name += " (";
      room_name += it->second->Name();
      room_name += ")";

      result.push_back(room_name);
    }
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
  }
}


int GamePoints() {
  PacmanPtr pacman = FindMyPlayer();
  if (pacman != PacmanPtr())
    return pacman->GamePoints();
  return 0;
}


int GhostsInARow() {
  PacmanPtr pacman = FindMyPlayer();
  if (pacman != PacmanPtr())
    return pacman->GhostsInARow();
  return 0;
}


bool Invincible() {
  PacmanPtr pacman = FindMyPlayer();
  if (pacman != PacmanPtr())
    if (pacman->Invincible() == 1)
      return true;
  return false;
}


int LevelNumber() {
  PacmanPtr pacman = FindMyPlayer();
  if (pacman != PacmanPtr())
    return pacman->LevelNumber();
  return 0;
}


int RemainLives() {
  PacmanPtr pacman = FindMyPlayer();
  if (pacman != PacmanPtr())
    return pacman->RemainLives();
  return 0;
}


int TimeLeft() {
  PacmanPtr pacman = FindMyPlayer();
  if (pacman != PacmanPtr())
    return pacman->TimeLeft();
  return 0;
}


std::vector<std::vector<int> > Locations() {
  std::vector<std::vector<int> > result;

  PacmanPtr pacman = FindMyPlayer();
  if (pacman == PacmanPtr())
    return result;

  std::string locations = pacman->Locations();
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


std::vector<std::vector<int> > Level() {
  std::vector<std::vector<int> > result;

  PacmanPtr pacman = FindMyPlayer();
  if (pacman == PacmanPtr())
    return result;

  std::string level = pacman->Level();
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



