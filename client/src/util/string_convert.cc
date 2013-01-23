// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#include <string>

#include "util/string_convert.h"


const char kConvertBase = 'a';


// string data type change to set(int to char)
std::string Encode(const std::string &int_string) {
  std::string ret_value = "";

  for (size_t i = 0; i < int_string.size(); ++i) {
    ret_value += kConvertBase + int_string[i];
  }

  return ret_value;
}


// string data type chage to get (char to int)
std::string Decode(const std::string &char_string) {
  std::string ret_value = "";

  for (size_t i = 0; i < char_string.size(); ++i) {
    int data = char_string[i] - kConvertBase;
    ret_value += static_cast<char>(data);
  }

  return ret_value;
}
