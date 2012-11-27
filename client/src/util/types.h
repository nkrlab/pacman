// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef CLIENT_SRC_UTIL_TYPES_H_
#define CLIENT_SRC_UTIL_TYPES_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/weak_ptr.hpp>
#include <json_spirit.h>
#include <stdint.h>

#include <string>

namespace google {namespace protobuf {}}


namespace fun {

namespace json = json_spirit;
namespace protobuf = google::protobuf;

using boost::shared_ptr;
using boost::weak_ptr;
typedef boost::uuids::uuid Uuid;
typedef std::string string;

// Determines if the subsequent event handler would be called.
// There could be multiple event handlers registered for a single event.
// If the current event handler returns CONTINUE, the next event handler
// will be invoked. Otherwise, the event chain will stop at the current handler.
enum ChainAction {
  CONTINUE = 0,
  STOP,
};

}  // namespace fun

#define DECLARE_CLASS_PTR(CLS) \
  typedef shared_ptr<CLS> Ptr; \
  typedef shared_ptr<const CLS> ConstPtr; \
  static const Ptr kNullPtr; \
  \
  typedef weak_ptr<CLS> WeakPtr; \
  typedef weak_ptr<const CLS> ConstWeakPtr; \
  static const WeakPtr kNullWeakPtr;


#define DEFINE_CLASS_PTR(CLS) \
  const CLS::Ptr CLS::kNullPtr; \
  const CLS::WeakPtr CLS::kNullWeakPtr;


#define DECLARE_EVENT(CLS) \
  typedef shared_ptr<CLS> Ptr; \
  typedef shared_ptr<const CLS> ConstPtr; \
  typedef boost::function<fun::ChainAction (CLS::ConstPtr)> Handler;

#endif  // CLIENT_SRC_UTIL_TYPES_H_
