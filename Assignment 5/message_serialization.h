//header file for message serialization
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#ifndef MESSAGE_SERIALIZATION_H
#define MESSAGE_SERIALIZATION_H

#include "message.h"

namespace MessageSerialization {
  
  // Function to encode a Message object into a string format
  void encode(const Message &msg, std::string &encoded_msg);
  
  // Function to decode an encoded string back into a Message object
  void decode(const std::string &encoded_msg, Message &msg);
};

#endif // MESSAGE_SERIALIZATION_H
