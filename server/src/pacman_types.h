// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef PACMAN_SERVER_SRC_PACMAN_TYPES_H_
#define PACMAN_SERVER_SRC_PACMAN_TYPES_H_

#include <funapi/common/types.h>
#include <google/protobuf/message.h>

#include "pacman.h"

namespace pacman {

  using fun::string;
  using fun::shared_ptr;
  using fun::object::Pacman;
  using fun::object::PacmanPtr;
  using fun::object::PacmanPtrVector;
  using fun::object::PacmanPtrMap;

  namespace protobuf = google::protobuf;
}  // namespace pacman

#endif  // PACMAN_SERVER_SRC_PACMAN_TYPES_H_
