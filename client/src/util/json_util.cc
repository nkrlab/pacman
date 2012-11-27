// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <boost/foreach.hpp>

#include "util/json_util.h"


namespace fun {
namespace util {

void FromJson(const json::mArray &json_array, std::vector<int64_t> *integers) {
  BOOST_ASSERT(integers->empty() == true);
  BOOST_FOREACH(const json::mValue &element, json_array) {
    integers->push_back(element.get_int());
  }
}


void FromJson(const json::mArray &json_array, std::vector<string> *strings) {
  BOOST_ASSERT(strings->empty() == true);
  BOOST_FOREACH(const json::mValue &element, json_array) {
    strings->push_back(element.get_str());
  }
}


void ToJson(const std::vector<int64_t> &integers, json::mArray *json_array) {
  BOOST_ASSERT(json_array->empty() == true);
  BOOST_FOREACH(const int64_t &element, integers) {
    json_array->push_back(element);
  }
}


void ToJson(const std::vector<string> &strings, json::mArray *json_array) {
  BOOST_ASSERT(json_array->empty() == true);
  BOOST_FOREACH(const string &element, strings) {
    json_array->push_back(element);
  }
}


std::vector<string> FromJsonArray(const string &dump) {
  json::Value j;
  json::read(dump, j);
  BOOST_ASSERT(j.type() == json::array_type);
  json::Array j_array = j.get_array();
  std::vector<string> v;
  BOOST_FOREACH(const json::Value &element, j_array) {
    BOOST_ASSERT(element.type() == json::str_type);
    v.push_back(element.get_str());
  }
  return v;
}

}  // namespace util
}  // namespace fun
