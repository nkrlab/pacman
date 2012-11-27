// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef CLIENT_SRC_UTIL_JSON_UTIL_H_
#define CLIENT_SRC_UTIL_JSON_UTIL_H_

#include <boost/filesystem/fstream.hpp>
#include <glog/logging.h>

#include <string>
#include <vector>

#include "util/types.h"


namespace fun {
namespace util {

void FromJson(const json::mArray &json_array, std::vector<int64_t> *integers);
void FromJson(const json::mArray &json_array, std::vector<string> *strings);

void ToJson(const std::vector<int64_t> &integers, json::mArray *json_array);
void ToJson(const std::vector<string> &strings, json::mArray *json_array);

std::vector<string> FromJsonArray(const string &dump);

template <typename JsonValue>  // JsonValue in (json::Value, json::mValue, ..)
shared_ptr<const JsonValue> CreateJsonValueFromFile(
    const string &file_name) {
  static const shared_ptr<const JsonValue> kNullValuePtr(new JsonValue());

  std::ifstream ifile(file_name.c_str());
  if (not ifile.is_open()) {
    LOG(ERROR) << "Failed to open " << file_name << ". Skipping.";
    return kNullValuePtr;
  }

  LOG(INFO) << "Processing " << file_name;

  shared_ptr<JsonValue> v_ptr(new JsonValue());
  bool result = json::read(ifile, *v_ptr);
  ifile.close();
  if (result == false) {
    LOG(ERROR) << "Failed to load json from " << file_name << ". Skipping. ";
    return kNullValuePtr;
  }
  if (v_ptr->type() != json::obj_type) {
    LOG(ERROR) << file_name << " is not a json object. Skipping";
    return kNullValuePtr;
  }

  return v_ptr;
}

}  // namespace util
}  // namespace fun

#endif  // CLIENT_SRC_UTIL_JSON_UTIL_H_
