// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.
//
// Created by Isaac Jeon

#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/random.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <json_spirit.h>

#include <algorithm>
#include <cstring>
#include <utility>

#include "util/boost_util.h"


namespace fun {
namespace util {

const Uuid kNilUuid = {{0U, 0U, 0U, 0U,
                        0U, 0U, 0U, 0U,
                        0U, 0U, 0U, 0U,
                        0U, 0U, 0U, 0U}};


const json::mArray kNilUuidJson = ToJson(kNilUuid);


bool IsNilUuid(const char *bin_data) {
  BOOST_ASSERT(bin_data != NULL);
  return (memcmp(bin_data, kNilUuid.data, Uuid::static_size()) == 0);
}


string ToString(const Uuid &uuid) {
  return boost::uuids::to_string(uuid);
}


Uuid FromString(const string &uuid_string) {
  static boost::uuids::string_generator gen;
  return gen(uuid_string);
}


IntPair ToIntPair(const Uuid &uuid) {
  Uuid::const_iterator i = uuid.begin();
  int64_t high = static_cast<int64_t>(*i);  // data[0]
  high <<= 8;
  ++i;
  high |= static_cast<int64_t>(*i);  // data[1]
  high <<= 8;
  ++i;
  high |= static_cast<int64_t>(*i);  // data[2]
  high <<= 8;
  ++i;
  high |= static_cast<int64_t>(*i);  // data[3]
  high <<= 8;
  ++i;
  high |= static_cast<int64_t>(*i);  // data[4]
  high <<= 8;
  ++i;
  high |= static_cast<int64_t>(*i);  // data[5]
  high <<= 8;
  ++i;
  high |= static_cast<int64_t>(*i);  // data[6]
  high <<= 8;
  ++i;
  high |= static_cast<int64_t>(*i);  // data[7]

  ++i;
  int64_t low = static_cast<int64_t>(*i);  // data[8]
  low <<= 8;
  ++i;
  low |= static_cast<int64_t>(*i);  // data[9]
  low <<= 8;
  ++i;
  low |= static_cast<int64_t>(*i);  // data[10]
  low <<= 8;
  ++i;
  low |= static_cast<int64_t>(*i);  // data[11]
  low <<= 8;
  ++i;
  low |= static_cast<int64_t>(*i);  // data[12]
  low <<= 8;
  ++i;
  low |= static_cast<int64_t>(*i);  // data[13]
  low <<= 8;
  ++i;
  low |= static_cast<int64_t>(*i);  // data[14]
  low <<= 8;
  ++i;
  low |= static_cast<int64_t>(*i);  // data[15]

  return std::make_pair(high, low);
}


Uuid FromIntPair(const IntPair &ipair) {
  int64_t high = ipair.first;
  int64_t low = ipair.second;

  Uuid uuid;
  Uuid::iterator i = uuid.end();
  --i;
  *i = static_cast<uint8_t>(low & 0xFF);  // data[15]
  low >>= 8;
  --i;
  *i = static_cast<uint8_t>(low & 0xFF);  // data[14]
  low >>= 8;
  --i;
  *i = static_cast<uint8_t>(low & 0xFF);  // data[13]
  low >>= 8;
  --i;
  *i = static_cast<uint8_t>(low & 0xFF);  // data[12]
  low >>= 8;
  --i;
  *i = static_cast<uint8_t>(low & 0xFF);  // data[11]
  low >>= 8;
  --i;
  *i = static_cast<uint8_t>(low & 0xFF);  // data[10]
  low >>= 8;
  --i;
  *i = static_cast<uint8_t>(low & 0xFF);  // data[9]
  low >>= 8;
  --i;
  *i = static_cast<uint8_t>(low);  // data[8]

  --i;
  *i = static_cast<uint8_t>(high & 0xFF);  // data[7]
  high >>= 8;
  --i;
  *i = static_cast<uint8_t>(high & 0xFF);  // data[6]
  high >>= 8;
  --i;
  *i = static_cast<uint8_t>(high & 0xFF);  // data[5]
  high >>= 8;
  --i;
  *i = static_cast<uint8_t>(high & 0xFF);  // data[4]
  high >>= 8;
  --i;
  *i = static_cast<uint8_t>(high & 0xFF);  // data[3]
  high >>= 8;
  --i;
  *i = static_cast<uint8_t>(high & 0xFF);  // data[2]
  high >>= 8;
  --i;
  *i = static_cast<uint8_t>(high & 0xFF);  // data[1]
  high >>= 8;
  --i;
  *i = static_cast<uint8_t>(high);  // data[0]

  return uuid;
}


json::mArray ToJson(const Uuid &uuid) {
  IntPair p = ToIntPair(uuid);
  json::mArray array;
  array.push_back(p.first);
  array.push_back(p.second);
  return array;
}


Uuid FromJson(const json::mArray &array) {
  BOOST_ASSERT(array.size() == 2);
  json::mArray::const_iterator i = array.begin();
  int64_t high = i->get_int64();
  ++i;
  int64_t low = i->get_int64();
  return FromIntPair(IntPair(high, low));
}


void FromJson(const json::mArray &json_array, std::vector<Uuid> *uuids) {
  BOOST_ASSERT(uuids->empty() == true);
  BOOST_FOREACH(const json::mValue &element, json_array) {
    uuids->push_back(FromJson(element.get_array()));
  }
}


void FromJson(const json::mObject &json_object,
              std::map<string, Uuid> *uuids) {
  BOOST_ASSERT(uuids->empty() == true);
  BOOST_FOREACH(const json::mObject::value_type &element, json_object) {
    uuids->insert(std::make_pair(element.first,
                                 FromJson(element.second.get_array())));
  }
}


void ToJson(const std::vector<Uuid> &uuids, json::mArray *json_array) {
  BOOST_ASSERT(json_array->empty() == true);
  BOOST_FOREACH(const Uuid &element, uuids) {
    json_array->push_back(ToJson(element));
  }
}


void ToJson(const std::map<string, Uuid> &uuids, json::mObject *json_object) {
  BOOST_ASSERT(json_object->empty() == true);
  typedef std::map<string, Uuid> UuidMap;
  BOOST_FOREACH(const UuidMap::value_type &element, uuids) {
    json_object->insert(std::make_pair(element.first,
                                       ToJson(element.second)));
  }
}


int64_t Random(int64_t min, int64_t max) {  // [min, max] (max also inclusive.)
  static boost::mt19937 gen;

  return boost::uniform_int<int64_t>(min, max)(gen);
}

}  // namespace util
}  // namespace fun
