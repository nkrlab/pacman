// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

#ifndef CLIENT_SRC_UTIL_STRING_CONVERT_H_
#define CLIENT_SRC_UTIL_STRING_CONVERT_H_

#include <string>


// string data type change to set(int to char)
std::string Encode(const std::string &int_string);

// string data type chage to get (char to int)
std::string Decode(const std::string &char_string);


#endif  // CLIENT_SRC_UTIL_STRING_CONVERT_H_
