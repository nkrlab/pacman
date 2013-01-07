// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.


#include <funapi/account/multicaster.h>
#include <funapi/api/clock.h>
#include <funapi/object/object.h>
#include <funapi/common/serialization/bson_archive.h>
#include <funapi/system/logging.h>

#include "funapi_event_handlers.h"
#include "pacman_event_handlers.h"
#include "pacman_types.h"


namespace pacman {

const int64_t kWorldTickMicrosecond = 180000;  // 0.18 second.
const fun::string kWorldObjectModelName("World");
const fun::string kAccountObjectModelName("Player");

const fun::string kRoomChannelName("room");
const fun::string kRoomChannelSubId("1");

std::set<fun::Account::Ptr> live_accounts;
time_t prev_send_second = 0;


void OnWorldReady(int64_t /*now_nanosec*/) {
  the_world = Pacman::CreateNew(kWorldObjectModelName);
  the_world->EnterChannel(kRoomChannelName, kRoomChannelSubId);
}


void OnWorldTick(int64_t now_nanosec) {
  GameTick();

  // check prev send time
  time_t second = now_nanosec / (1000 * 1000);
  if (prev_send_second != second) {
    SendTimeMessage(now_nanosec);
    prev_send_second = second;
  }
}


fun::Object::Ptr CreateObject(const fun::string &model) {
  return Pacman::CreateNew(model);
}


fun::Object::Ptr DeserializeObject(const fun::string &serial) {
  fun::BsonArchive::Ptr archive_ptr =
      fun::BsonArchive::CreateFromSerialized(serial);
  return Pacman::CreateFromSerialized(*archive_ptr);
}


void OnAccountLogin(const fun::Account::Ptr &account) {
  PacmanPtr player = Pacman::Cast(account->object());
  const string &player_name = player->name();
  fun::Multicaster::Get().EnterChannel(kRoomChannelName, kRoomChannelSubId,
                                       account);
  live_accounts.insert(account);

  FUN_LOG_INFO << "account login[" << account->account_id()
               << "] player name[" << player_name
               << "]";
}


void OnAccountLogout(const fun::Account::Ptr &account) {
  live_accounts.erase(account);
  PacmanPtr player = Pacman::Cast(account->object());
  const string &player_name = player->name();
  ErasePlayer(player_name);
  fun::Multicaster::Get().LeaveChannel(kRoomChannelName, kRoomChannelSubId,
                                       account);

  FUN_LOG_INFO << "account logout[" << account->account_id()
               << "] player name[" << player_name
               << "]";
}


void OnAccountTimeout(const fun::Account::Ptr &account) {
  OnAccountLogout(account);
}


void OnAccountMessage(const fun::Account::Ptr &account,
                      const ::ClientAppMessage &msg) {
  PacmanPtr player = Pacman::Cast(account->object());

  ::ClientAppMessageType::Type msg_type = msg.GetExtension(client_message_type);
  switch (msg_type) {
    case ::ClientAppMessageType::kLoadLevel: {
      OnLoadLevel(player, msg.GetExtension(load_level));
      break;
    }
    case ::ClientAppMessageType::kPacmanMove: {
      OnPacmanMove(player, msg.GetExtension(pacman_move));
      break;
    }
    default: {
      FUN_LOG_ERR << "Unknown client message type: " << (int64_t)msg_type;
      break;
    }
  }
}

}  // namespace pacman
