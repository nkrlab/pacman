// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <boost/foreach.hpp>
#include <utility>

#include "util/boost_util.h"
#include "util/json_archive.h"
#include "util/json_util.h"


namespace fun {

DEFINE_CLASS_PTR(JsonArchive)


JsonArchive::JsonArchive() : Archive(), obj_() {
}


JsonArchive::~JsonArchive() {
}


JsonArchive::Ptr JsonArchive::CreateFromSerialized(const string &serialized) {
  json::mValue v;
  json::read(serialized, v);
  if (v.type() == json::null_type) {
    return JsonArchive::Ptr(new JsonArchive());
  }
  return JsonArchive::Ptr(new JsonArchive(v.get_obj()));
}


Archive::ReaderPtr JsonArchive::CreateReader() const {
  return Archive::ReaderPtr(new JsonArchive::Reader(obj_));
}


Archive::WriterPtr JsonArchive::CreateWriter() {
  return Archive::WriterPtr(new JsonArchive::Writer(&obj_));
}


bool JsonArchive::IsEmpty() const {
  return obj_.empty();
}


string JsonArchive::Serialize() const {
  return json::write(obj_);
}


string JsonArchive::ToString() const {
  return json::write(obj_);
}


JsonArchive::JsonArchive(const json::mObject &obj) : Archive(), obj_(obj) {
}


JsonArchive::Reader::Reader(const json::mObject &obj) : obj_(obj) {
}


JsonArchive::Reader::~Reader() {
}


void JsonArchive::Reader::FromJson(const json::mArray &json_array,
                                   ArchivePtrArray *archive_array) {
  BOOST_FOREACH(const json::mValue &element, json_array) {
    JsonArchive::Ptr archive(new JsonArchive(element.get_obj()));
    archive_array->push_back(archive);
  }
}


void JsonArchive::Reader::FromJson(const json::mObject &json_object,
                                   ArchivePtrObject *archive_object) {
  BOOST_FOREACH(const json::mObject::value_type &element, json_object) {
    JsonArchive::Ptr archive(new JsonArchive(element.second.get_obj()));
    archive_object->insert(std::make_pair(element.first, archive));
  }
}


UuidPtr JsonArchive::Reader::ReadUuid(const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    return UuidPtr();
  }
  const json::mArray &value = i->second.get_array();

  // TODO(khpark): prevent redundant copy.
  // return util::FromJson(value);  // return Uuid -> UuidPtr
  return UuidPtr(new Uuid(util::FromJson(value)));
}


IntegerPtr JsonArchive::Reader::ReadInteger(const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullIntegerPtr;
    return IntegerPtr();
  }
  const int64_t &value = i->second.get_int();
  return IntegerPtr(new int64_t(value));
}


StringPtr JsonArchive::Reader::ReadString(const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullStringPtr;
    return StringPtr();
  }
  const string &value = i->second.get_str();
  return StringPtr(new string(value));
}


ArchivePtr JsonArchive::Reader::ReadArchive(const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    return JsonArchive::kNullPtr;
  }
  const json::mObject &value = i->second.get_obj();
  return JsonArchive::Ptr(new JsonArchive(value));
}


IntegerArrayPtr JsonArchive::Reader::ReadIntegerArray(
    const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullIntegerArrayPtr;
    return IntegerArrayPtr();
  }
  const json::mArray &arr = i->second.get_array();
  IntegerArray *v_ptr = new IntegerArray();
  util::FromJson(arr, v_ptr);
  return IntegerArrayPtr(v_ptr);
}


StringArrayPtr JsonArchive::Reader::ReadStringArray(const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullStringArrayPtr;
    return StringArrayPtr();
  }
  const json::mArray &arr = i->second.get_array();
  StringArray *v_ptr = new StringArray();
  util::FromJson(arr, v_ptr);
  return StringArrayPtr(v_ptr);
}


ArchivePtrArrayPtr JsonArchive::Reader::ReadArchivePtrArray(
    const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullArchivePtrArrayPtr;
    return ArchivePtrArrayPtr();
  }
  const json::mArray &arr = i->second.get_array();
  ArchivePtrArray *v_ptr = new ArchivePtrArray();
  FromJson(arr, v_ptr);
  return ArchivePtrArrayPtr(v_ptr);
}


ArchivePtrObjectPtr JsonArchive::Reader::ReadArchivePtrObject(
    const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullArchivePtrObjectPtr;
    return ArchivePtrObjectPtr();
  }
  const json::mObject &obj = i->second.get_obj();
  ArchivePtrObject *m_ptr = new ArchivePtrObject();
  FromJson(obj, m_ptr);
  return ArchivePtrObjectPtr(m_ptr);
}


UuidArrayPtr JsonArchive::Reader::ReadUuidArray(const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullUuidArrayPtr;
    return UuidArrayPtr();
  }
  const json::mArray &arr = i->second.get_array();
  UuidArray *v_ptr = new UuidArray();
  util::FromJson(arr, v_ptr);
  return UuidArrayPtr(v_ptr);
}


UuidObjectPtr JsonArchive::Reader::ReadUuidObject(
    const string &key) const {
  json::mObject::const_iterator i = obj_.find(key);
  if (i == obj_.end()) {
    // TODO(khpark): return kNullUuidObjectPtr;
    return UuidObjectPtr();
  }
  const json::mObject &obj = i->second.get_obj();
  UuidObject *m_ptr = new UuidObject();
  util::FromJson(obj, m_ptr);
  return UuidObjectPtr(m_ptr);
}


JsonArchive::Writer::Writer(json::mObject *obj) : obj_(obj) {
}


JsonArchive::Writer::~Writer() {
}


void JsonArchive::Writer::ToJson(const SerializablePtrArray &serial_array,
                                 json::mArray *json_array) {
  BOOST_FOREACH(const Serializable::Ptr &element, serial_array) {
    JsonArchive archive;
    element->SerializeTo(&archive);
    json_array->push_back(archive.obj_);
  }
}


void JsonArchive::Writer::ToJson(const SerializablePtrObject &serial_array,
                                 json::mObject *json_array) {
  BOOST_FOREACH(const SerializablePtrObject::value_type &element,
                serial_array) {
    JsonArchive archive;
    element.second->SerializeTo(&archive);
    json_array->insert(std::make_pair(element.first, archive.obj_));
  }
}


void JsonArchive::Writer::WriteUuid(const string &key, const Uuid &value) {
  json::mArray arr = util::ToJson(value);
  obj_->insert(std::make_pair(key, arr));
}


void JsonArchive::Writer::WriteInteger(const string &key,
                                       const int64_t &value) {
  obj_->insert(std::make_pair(key, value));
}


void JsonArchive::Writer::WriteString(const string &key, const string &value) {
  obj_->insert(std::make_pair(key, value));
}


void JsonArchive::Writer::WriteArchive(const string &key,
                                       const Serializable &value) {
  JsonArchive json_obj;
  value.SerializeTo(&json_obj);
  obj_->insert(std::make_pair(key, json_obj.obj_));
}


void JsonArchive::Writer::WriteIntegerArray(const string &key,
                                            const IntegerArray &value) {
  json::mArray arr;
  util::ToJson(value, &arr);
  obj_->insert(std::make_pair(key, arr));
}


void JsonArchive::Writer::WriteStringArray(const string &key,
                                           const StringArray &value) {
  json::mArray arr;
  util::ToJson(value, &arr);
  obj_->insert(std::make_pair(key, arr));
}


void JsonArchive::Writer::WriteArchivePtrArray(const string &key,
    const SerializablePtrArray &value) {
  json::mArray arr;
  ToJson(value, &arr);
  obj_->insert(std::make_pair(key, arr));
}


void JsonArchive::Writer::WriteArchivePtrObject(const string &key,
    const SerializablePtrObject &value) {
  json::mObject obj;
  ToJson(value, &obj);
  obj_->insert(std::make_pair(key, obj));
}


void JsonArchive::Writer::WriteUuidArray(const string &key,
                                         const UuidArray &value) {
  json::mArray arr;
  util::ToJson(value, &arr);
  obj_->insert(std::make_pair(key, arr));
}


void JsonArchive::Writer::WriteUuidObject(const string &key,
    const UuidObject &value) {
  json::mObject obj;
  util::ToJson(value, &obj);
  obj_->insert(std::make_pair(key, obj));
}

}  // namespace fun
