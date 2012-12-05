// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.


#include <funapi/api/clock.h>
#include <funapi/object/object.h>
#include <funapi/common/serialization/bson_archive.h>
#include <funapi/system/logging.h>

#include "funapi_event_handlers.h"
#include "pacman_event_handlers.h"
#include "pacman_types.h"


namespace pacman {

const int64_t kWorldTickMicrosecond = 1000000;  // 1 second.
const fun::string kWorldObjectModelName("World");
const fun::string kAccountObjectModelName("Player");


void OnWorldReady(int64_t /*now_nanosec*/) {
  the_world = Pacman::CreateNew(kWorldObjectModelName);
}


void OnWorldTick(int64_t /*now_nanosec*/) {
  // 현재로써는 클라이언트의 요청이 있을때 Tick을 수행하므로 여기서는
  // 할일이 없다.
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

  FUN_LOG_INFO << "account login[" << account->account_id()
               << "] player name[" << player_name
               << "]";
}


void OnAccountLogout(const fun::Account::Ptr &account) {
  PacmanPtr player = Pacman::Cast(account->object());
  const string &player_name = player->name();
  ErasePlayer(player_name);

  FUN_LOG_INFO << "account logout[" << account->account_id()
               << "] player name[" << player_name
               << "]";
}


void OnAccountTimeout(const fun::Account::Ptr &account) {
  OnAccountLogout(account);
}


void OnAccountMessage(const fun::Account::Ptr &account,
                      const ::ClientAppMessage &msg) {
  the_current_account = account;
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
    case ::ClientAppMessageType::kRequestTick: {
      OnRequestTick(player, msg.GetExtension(request_tick));
      break;
    }
    default: {
      FUN_LOG_ERR << "Unknown client message type: " << (int64_t)msg_type;
      break;
    }
  }
}

}  // namespace pacman
