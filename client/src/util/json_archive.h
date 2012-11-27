// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef CLIENT_SRC_UTIL_JSON_ARCHIVE_H_
#define CLIENT_SRC_UTIL_JSON_ARCHIVE_H_

#include <string>

#include "util/types.h"
#include "util/archive.h"


namespace fun {
/// JsonArchive 는 내부 자료구조가 JSON 형태로 되어있는 Archive 이다.
/// 상세한 내용은 Archive 를 참고한다.
/// @see Archive, BsonArchive
class JsonArchive : public Archive {
 public:
  DECLARE_CLASS_PTR(JsonArchive)

  class Reader;
  class Writer;

  JsonArchive();
  virtual ~JsonArchive();

  /// 시리얼라이제이션된 string 으로부터 JsonArchive 를 생성한다.
  /// @param serialized 시리얼라이제이션된 string.
  /// @return 생성된 JsonArchive.
  static Ptr CreateFromSerialized(const string &serialized);

  virtual Archive::ReaderPtr CreateReader() const;
  virtual Archive::WriterPtr CreateWriter();

  virtual bool IsEmpty() const;
  virtual string Serialize() const;
  virtual string ToString() const;

 private:
  explicit JsonArchive(const json::mObject &obj);

  json::mObject obj_;
};


/// JsonArchive::Reader 는 JsonArchive 를 위한 Archive::Reader 이다.
/// 상세한 내용은 Archive::Reader 를 참고한다.
/// @see Archive::Reader, BsonArchive::Reader
class JsonArchive::Reader : public Archive::Reader {
 public:
  explicit Reader(const json::mObject &obj);
  virtual ~Reader();

  static void FromJson(const json::mArray &json_array,
                       ArchivePtrArray *archive_array);
  static void FromJson(const json::mObject &json_array,
                       ArchivePtrObject *archive_array);

  virtual UuidPtr ReadUuid(const string &key) const;
  virtual IntegerPtr ReadInteger(const string &key) const;
  virtual StringPtr ReadString(const string &key) const;
  virtual ArchivePtr ReadArchive(const string &key) const;
  virtual IntegerArrayPtr ReadIntegerArray(const string &key) const;
  virtual StringArrayPtr ReadStringArray(const string &key) const;
  virtual ArchivePtrArrayPtr ReadArchivePtrArray(const string &key) const;
  virtual ArchivePtrObjectPtr ReadArchivePtrObject(const string &key) const;
  virtual UuidArrayPtr ReadUuidArray(const string &key) const;
  virtual UuidObjectPtr ReadUuidObject(const string &key) const;

 private:
  const json::mObject &obj_;
};


/// JsonArchive::Writer 는 JsonArchive 를 위한 Archive::Writer 이다.
/// 상세한 내용은 Archive::Writer 를 참고한다.
/// @see Archive::Writer, BsonArchive::Writer
class JsonArchive::Writer : public Archive::Writer {
 public:
  explicit Writer(json::mObject *obj);
  virtual ~Writer();

  static void ToJson(const SerializablePtrArray &serial_array,
                     json::mArray *json_array);
  static void ToJson(const SerializablePtrObject &serial_object,
                     json::mObject *json_object);

  virtual void WriteUuid(const string &key, const Uuid &value);
  virtual void WriteInteger(const string &key, const int64_t &value);
  virtual void WriteString(const string &key, const string &value);
  virtual void WriteArchive(const string &key, const Serializable &value);
  virtual void WriteIntegerArray(const string &key,
                                 const IntegerArray &value);
  virtual void WriteStringArray(const string &key,
                                const StringArray &value);
  virtual void WriteArchivePtrArray(const string &key,
                                    const SerializablePtrArray &value);
  virtual void WriteArchivePtrObject(const string &key,
                                     const SerializablePtrObject &value);
  virtual void WriteUuidArray(const string &key, const UuidArray &value);
  virtual void WriteUuidObject(const string &key, const UuidObject &value);

 private:
  json::mObject *obj_;
};

}  // namespace fun

#endif  // CLIENT_SRC_UTIL_JSON_ARCHIVE_H_
