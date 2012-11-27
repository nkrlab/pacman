// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef CLIENT_SRC_UTIL_SERIALIZABLE_H_
#define CLIENT_SRC_UTIL_SERIALIZABLE_H_

#include <map>
#include <string>
#include <vector>

#include "src/util/types.h"


namespace fun {

class Archive;

/// Serializable class 는 Archive object 에 serialization 할 수 있는 class 이다.
/// serialization 이 가능해야하는 class 들은 Serializable class 를 상속받은 후,
/// SerializeTo method 를 구현해야한다.
///
/// @see Archive
class Serializable {
 public:
  typedef shared_ptr<Serializable> Ptr;

  virtual ~Serializable() = 0;

  /// Serializable 의 내용을 Archive 에 시리얼라이제이션한다.
  /// @param archive Serializable 의 내용을
  ///        시리얼라이제이션해서 보관할 Archive.
  ///        호출 당시에는 archive 가 비어있어야하며,
  ///        호출이 끝나면 archive 에 데이터가 채워지게 된다.
  virtual void SerializeTo(Archive *archive) const = 0;
};


typedef std::vector<Serializable::Ptr> SerializablePtrArray;
typedef shared_ptr<SerializablePtrArray> SerializablePtrArrayPtr;

typedef std::map<string, Serializable::Ptr> SerializablePtrObject;
typedef shared_ptr<SerializablePtrObject> SerializablePtrObjectPtr;


inline Serializable::~Serializable() {
}

}  // namespace fun

#endif  // CLIENT_SRC_UTIL_SERIALIZABLE_H_
