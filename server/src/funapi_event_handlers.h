// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef PACMAN_SERVER_SRC_FUNAPI_EVENT_HANDLERS_H_
#define PACMAN_SERVER_SRC_FUNAPI_EVENT_HANDLERS_H_

#include <funapi/account/account_event_handler_registry.h>
#include <funapi/account/account.h>
#include <funapi/account/object_creator_registry.h>
#include <funapi/object/object.h>
#include <funapi/common/types.h>

#include "app_messages.pb.h"

namespace pacman {

  // server 의 timer event 발생 주기.
  extern const int64_t kWorldTickMicrosecond;

  // world object 용 model 의 이름.
  extern const fun::string kWorldObjectModelName;

  // account object 용 model 의 이름.
  extern const fun::string kAccountObjectModelName;

  // server 가 최초 뜰 때만 불린다.
  void OnWorldReady(int64_t now_nanosec);

  // server 의 timer 를 통해 지정된 주기(kTickNanosecond)로 불린다.
  void OnWorldTick(int64_t now_nanosec);

  // object creators.
  fun::object::Object::Ptr CreateObject(const fun::string &model);
  fun::object::Object::Ptr DeserializeObject(const fun::string &serial);

  // account event handlers.
  void OnAccountLogin(const fun::Account::Ptr &account);
  void OnAccountLogout(const fun::Account::Ptr &account);
  void OnAccountTimeout(const fun::Account::Ptr &account);
  void OnAccountMessage(const fun::Account::Ptr &account,
                        const ::ClientAppMessage &msg);

}  // namespace pacman

#endif  // PACMAN_SERVER_SRC_FUNAPI_EVENT_HANDLERS_H_
