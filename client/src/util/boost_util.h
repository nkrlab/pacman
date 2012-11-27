// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.
//
// Created by Isaac Jeon

#ifndef CLIENT_SRC_UTIL_BOOST_UTIL_H_
#define CLIENT_SRC_UTIL_BOOST_UTIL_H_

#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "util/types.h"


namespace fun {
namespace util {

extern const Uuid kNilUuid;
extern const json::mArray kNilUuidJson;

bool IsNilUuid(const char *bin_data);

// DEPRECATED. Cf. boost::uuids::to_string() in uuid_io.hpp
string ToString(const Uuid &uuid);
Uuid FromString(const string &uuid_string);

typedef std::pair<int64_t, int64_t> IntPair;
IntPair ToIntPair(const Uuid &uuid);
Uuid FromIntPair(const IntPair &ipair);

Uuid FromJson(const json::mArray &array);
json::mArray ToJson(const Uuid &uuid);

void FromJson(const json::mArray &json_array, std::vector<Uuid> *uuids);
void FromJson(const json::mObject &json_object, std::map<string, Uuid> *uuids);

void ToJson(const std::vector<Uuid> &uuids, json::mArray *json_array);
void ToJson(const std::map<string, Uuid> &uuids, json::mObject *json_object);

int64_t Random(int64_t min, int64_t max);  // [min, max] (max also inclusive.)


/// case-independent (ci) compare_less binary function
struct nocase_compare
    : public std::binary_function<unsigned char, unsigned char, bool> {
  bool operator()(const unsigned char c1, const unsigned char c2) const {
    return tolower(c1) < tolower(c2);
  }
};


/// case-independent (ci) string less_than, returns true if s1 < s2
struct ci_less : std::binary_function<string, string, bool> {
  bool operator()(const string &s1, const string &s2) const {
    return lexicographical_compare
        (s1.begin(), s1.end(),  // source range
         s2.begin(), s2.end(),  // dest range
         nocase_compare());  // comparison
  }
};

}  // namespace util
}  // namespace fun

#endif  // CLIENT_SRC_UTIL_BOOST_UTIL_H_
