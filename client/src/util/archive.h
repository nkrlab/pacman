// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef CLIENT_SRC_UTIL_ARCHIVE_H_
#define CLIENT_SRC_UTIL_ARCHIVE_H_

#include <map>
#include <string>
#include <vector>

#include "util/types.h"
#include "util/serializable.h"


namespace fun {

typedef shared_ptr<Uuid> UuidPtr;
typedef shared_ptr<int64_t> IntegerPtr;
typedef shared_ptr<string> StringPtr;

class Archive;
typedef shared_ptr<Archive> ArchivePtr;

typedef std::vector<int64_t> IntegerArray;
typedef std::vector<string> StringArray;
typedef std::vector<ArchivePtr> ArchivePtrArray;
typedef std::map<string, ArchivePtr> ArchivePtrObject;
typedef std::vector<Uuid> UuidArray;
typedef std::map<string, Uuid> UuidObject;

typedef shared_ptr<IntegerArray> IntegerArrayPtr;
typedef shared_ptr<StringArray> StringArrayPtr;
typedef shared_ptr<ArchivePtrArray> ArchivePtrArrayPtr;
typedef shared_ptr<ArchivePtrObject> ArchivePtrObjectPtr;
typedef shared_ptr<UuidArray> UuidArrayPtr;
typedef shared_ptr<UuidObject> UuidObjectPtr;


/// Archive class 는 serializable object 의 serialized data 를 갖고 있는
/// class 이다.
/// Archive class 는 abstract class 이며 실제 serialization 을 위해서는
/// Archive class 를 상속받은 implementation class 가 필요하고,
/// serialization 방법 역시 implementation class 안에서 결정된다.
///
/// Archive class 의 instance 에 access 하려면,
/// Archive::Reader 와 Archive::Writer class 의 instance 가 필요하다.
/// Reader 와 Writer class 역시 abstract class 이므로,
/// 역시 각각 implementation class 들이 필요하다.
///
/// Serializable class 의 implementation class 안에서
/// 추상적인 형태인 Archive object 를 통해 serialization 하게 되면,
/// Serializable class 와 실제 Archive object 의 format 사이에
/// dependency 가 생기지 않게 된다.
/// 즉, 실제 Archive class 의 format 이나 implementation 이 변경되더라도
/// Serializable class 의 구현에는 영향을 주지 않게 된다.
///
/// @see JsonArchive, BsonArchive, Serializable, Object
class Archive {
 public:
  typedef shared_ptr<Archive> Ptr;

  class Reader;
  class Writer;
  typedef shared_ptr<Reader> ReaderPtr;
  typedef shared_ptr<Writer> WriterPtr;

  virtual ~Archive() = 0;

  /// Reader 를 생성한다.
  /// @return 생성된 Reader.
  virtual ReaderPtr CreateReader() const = 0;

  /// Writer 를 생성한다.
  /// @return 생성된 Writer.
  virtual WriterPtr CreateWriter() = 0;

  /// Archive 에 데이터가 있는지 검사한다.
  /// @return 데이터가 있으면 true, 없으면 false.
  virtual bool IsEmpty() const = 0;

  /// Archive 를 시리얼라이제이션한다.
  /// @return 바이너리 형태로 시리얼라이제이션된 string.
  // TODO(brucepark) Serialize 복사비용을 줄이기 위해
  // string을 리턴하지말고 인자로 string을 받는다.
  virtual string Serialize() const = 0;

  /// Archive 의 내용을 사람이 읽을 수 있는 텍스트의 형태로 생성한다.
  /// 주로 디버깅 목적으로 사용된다.
  /// @return 생성된 텍스트.
  virtual string ToString() const;

 protected:
  Archive();
};


inline Archive::~Archive() {
}


inline string Archive::ToString() const {
  return "";
}


inline Archive::Archive() {
}


/// Archive::Reader 클래스는 Archive 로부터 데이터를 읽어올 때 사용한다.
class Archive::Reader {
 public:
  virtual ~Reader() {}

  /// Archive 에 저장된 uuid 값을 읽어온다.
  /// @param key 읽어올 uuid 이름.
  /// @return 읽어온 uuid 값.
  virtual UuidPtr ReadUuid(const string &key) const = 0;

  /// Archive 에 저장된 integer 값을 읽어온다.
  /// @param key 읽어올 integer 이름.
  /// @return 읽어온 integer 값.
  virtual IntegerPtr ReadInteger(const string &key) const = 0;

  /// Archive 에 저장된 string 값을 읽어온다.
  /// @param key 읽어올 string 이름.
  /// @return 읽어온 string 값.
  virtual StringPtr ReadString(const string &key) const = 0;

  /// Archive 에 저장된 Archive 값을 읽어온다.
  /// 참고로, Archive 안에 다른 Archive 들을 포함할 수 있다.
  /// (단, Archive 의 타입은 모두 동일해야한다.)
  /// @param key 읽어올 Archive 이름.
  /// @return 읽어온 Archive 값.
  virtual ArchivePtr ReadArchive(const string &key) const = 0;

  /// Archive 에 저장된 integer array 값을 읽어온다.
  /// integer array 란 integer 값들의 집합이다.
  /// @param key 읽어올 integer array 이름.
  /// @return 읽어온 integer array 값.
  virtual IntegerArrayPtr ReadIntegerArray(const string &key) const = 0;

  /// Archive 에 저장된 string array 값을 읽어온다.
  /// string array 란 string 값들의 집합이다.
  /// @param key 읽어올 string array 이름.
  /// @return 읽어온 string array 값.
  virtual StringArrayPtr ReadStringArray(const string &key) const = 0;

  /// Archive 에 저장된 Archive array 를 읽어온다.
  /// Archive array 란 Archive 값들의 집합이다.
  /// @param key 읽어올 Archive array 이름.
  /// @return 읽어온 Archive array 값.
  virtual ArchivePtrArrayPtr ReadArchivePtrArray(const string &key) const = 0;

  /// Archive 에 저장된 Archive object 값을 읽어온다.
  /// Archive object 란 Archive 이름-값 쌍의 집합이다.
  /// @param key 읽어올 Archive object 이름.
  /// @return 읽어온 Archive object 값.
  virtual ArchivePtrObjectPtr ReadArchivePtrObject(const string &key) const = 0;

  /// Archive 에 저장된 uuid array 값을 읽어온다.
  /// uuid array 란 uuid 값들의 집합이다.
  /// @param key 읽어올 uuid array 이름.
  /// @return 읽어온 uuid array 값.
  virtual UuidArrayPtr ReadUuidArray(const string &key) const = 0;

  /// Archive 에 저장된 uuid object 값을 읽어온다.
  /// uuid object 란 uuid 이름-값 쌍의 집합이다.
  /// @param key 읽어올 uuid object 이름.
  /// @return 읽어온 uuid object 값.
  virtual UuidObjectPtr ReadUuidObject(const string &key) const = 0;
};


/// Archive::Writer 클래스는 Archive 에 데이터를 기록할 때 사용한다.
class Archive::Writer {
 public:
  virtual ~Writer() {}

  /// Archive 에 uuid 값을 기록한다.
  /// @param key 저장할 uuid 이름.
  /// @param value 저장할 uuid 값.
  virtual void WriteUuid(const string &key, const Uuid &value) = 0;

  /// Archive 에 integer 값을 기록한다.
  /// @param key 저장할 integer 이름.
  /// @param value 저장할 integer 값.
  virtual void WriteInteger(const string &key, const int64_t &value) = 0;

  /// Archive 에 string 값을 기록한다.
  /// @param key 저장할 string 이름.
  /// @param value 저장할 string 값.
  virtual void WriteString(const string &key, const string &value) = 0;

  /// Archive 에 Archive 값을 기록한다.
  /// 참고로, Archive 안에 다른 Archive 들을 포함할 수 있다.
  /// (단, Archive 의 타입은 모두 동일해야한다.)
  /// @param key 저장할 Archive 이름.
  /// @param value 저장할 Archive 값.
  virtual void WriteArchive(const string &key, const Serializable &value) = 0;

  /// Archive 에 integer array 값을 기록한다.
  /// integer array 란 integer 값들의 집합이다.
  /// @param key 저장할 integer array 이름.
  /// @param value 저장할 integer array 값.
  virtual void WriteIntegerArray(const string &key,
                                 const IntegerArray &value) = 0;

  /// Archive 에 string array 값을 기록한다.
  /// string array 란 string 값들의 집합이다.
  /// @param key 저장할 string array 이름.
  /// @param value 저장할 string array 값.
  virtual void WriteStringArray(const string &key,
                                const StringArray &value) = 0;

  /// Archive 에 Archive array 값을 기록한다.
  /// Archive array 란 Archive 값들의 집합이다.
  /// @param key 저장할 Archive array 이름.
  /// @param value 저장할 Archive array 값.
  virtual void WriteArchivePtrArray(const string &key,
                                    const SerializablePtrArray &value) = 0;

  /// Archive 에 Archive object 값을 기록한다.
  /// Archive object 란 Archive 이름-값 쌍의 집합이다.
  /// @param key 저장할 Archive object 이름.
  /// @param value 저장할 Archive object 값.
  virtual void WriteArchivePtrObject(const string &key,
                                     const SerializablePtrObject &value) = 0;

  /// Archive 에 uuid array 값을 기록한다.
  /// uuid array 란 uuid 값들의 집합이다.
  /// @param key 저장할 uuid array 이름.
  /// @param value 저장할 uuid array 값.
  virtual void WriteUuidArray(const string &key, const UuidArray &value) = 0;

  /// Archive 에 uuid object 값을 기록한다.
  /// uuid object 란 uuid 이름-값 쌍의 집합이다.
  /// @param key 저장할 uuid object 이름.
  /// @param value 저장할 uuid object 값.
  virtual void WriteUuidObject(const string &key, const UuidObject &value) = 0;

  /// Archive 의 종류에 따라 serialization 후 flush 가 필요한 경우가 있다.
  /// 예를 들어 JsonArchive 는 flush 가 필요 없으나
  /// BsonArchive 는 flush 가 필요하다.
  virtual void End();
};


inline void Archive::Writer::End() {
}

}  // namespace fun

#endif  // CLIENT_SRC_UTIL_ARCHIVE_H_
