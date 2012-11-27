// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef SERVER_SRC_PACMAN_TYPES_H_
#define SERVER_SRC_PACMAN_TYPES_H_

#include <funapi/common/types.h>
#include <google/protobuf/message.h>

#include "pacman.h"

namespace pacman {

  using fun::string;
  using fun::shared_ptr;
  using fun::Pacman;
  using fun::PacmanPtr;
  using fun::PacmanPtrVector;
  using fun::PacmanPtrMap;

  namespace protobuf = google::protobuf;
}  // namespace pacman

#endif  // SERVER_SRC_PACMAN_TYPES_H_
