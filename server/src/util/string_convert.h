// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef SERVER_SRC_UTIL_STRING_CONVERT_H_
#define SERVER_SRC_UTIL_STRING_CONVERT_H_

#include <string>


namespace pacman {

// string data type change to set(int to char)
std::string Encode(const std::string &int_string);

// string data type chage to get (char to int)
std::string Decode(const std::string &char_string);

}  // namespace pacman


#endif  // SERVER_SRC_UTIL_STRING_CONVERT_H_
