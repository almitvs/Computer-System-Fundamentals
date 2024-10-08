//implememtation of get function client
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include <iostream>
#include <cstring>
#include <set>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include <sstream>

int main(int argc, char **argv) {
  //check for number of arguments
  if (argc != 6) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    //exit if the usage is incorrect
    return 1;
  }

  //parse the comand-line arguments
  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  //try to connect to the server
  int clientfd = open_clientfd(hostname.c_str(), port.c_str());
  if (clientfd < 0) {
    std::cerr << "Error: Failed to connect to server\n";
    //exit if the connection fails
    return 1;
  }

  //initialize the read buffer
  rio_t rio;
  Rio_readinitb(&rio, clientfd);
  
  // create and send login message
  Message login_msg = Message(MessageType::LOGIN, {username});
  std::string encoded_msg;
  MessageSerialization::encode(login_msg, encoded_msg);
  if (rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length()) < 0) {
    std::cerr << "Error: Failed to send LOGIN message\n";
    Close(clientfd);
    return 1;
  }

  // Read LOGIN response
  char response[MAXLINE];
  if (Rio_readlineb(&rio, response, MAXLINE) <= 0) {
    std::cerr << "Error: Failed to read LOGIN response\n";
    Close(clientfd);
    return 1;
  }
  
  // Decode the response
  Message response_msg;
  MessageSerialization::decode(response, response_msg);
  if (response_msg.get_message_type() == MessageType::FAILED || response_msg.get_message_type() == MessageType::ERROR) {
    std::cerr << "Error: " << response_msg.get_quoted_text() << std::endl;
    Close(clientfd);
    return 1;
  }
  
  // Send GET message
  Message get_msg = Message(MessageType::GET, {table, key});
  MessageSerialization::encode(get_msg, encoded_msg);
  if (rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length()) < 0) {
    std::cerr << "Error: Failed to send GET message\n";
    Close(clientfd);
    return 1;
  }
  
  // Read GET response
  if (Rio_readlineb(&rio, response, MAXLINE) <= 0) {
    std::cerr << "Error: Failed to read GET response\n";
    Close(clientfd);
    return 1;
  }
  
  // Decode the response
  MessageSerialization::decode(response, response_msg);
  if (response_msg.get_message_type() == MessageType::FAILED || response_msg.get_message_type() == MessageType::ERROR) {
    std::cerr << "Error: " << response_msg.get_quoted_text() << std::endl;
    Close(clientfd);
    return 1;
  }
  
  // Send TOP message
  Message top_msg = Message(MessageType::TOP, {});
  MessageSerialization::encode(top_msg, encoded_msg);
  if (rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length()) < 0) {
    std::cerr << "Error: Failed to send GET message\n";
    Close(clientfd);
    return 1;
  }
  
  // Read TOP response
  if (Rio_readlineb(&rio, response, MAXLINE) <= 0) {
    std::cerr << "Error: Failed to read GET response\n";
    Close(clientfd);
    return 1;
  }
  
  // Decode the response                                                                                                                                                                                     
  MessageSerialization::decode(response, response_msg);
  if (response_msg.get_message_type() == MessageType::FAILED || response_msg.get_message_type() == MessageType::ERROR) {
    std::cerr << "Error: " << response_msg.get_quoted_text() << std::endl;
    Close(clientfd);
    return 1;
  } else {
    std::cout << response_msg.get_value() << std::endl;
  }
  
  
  // Send BYE message
  Message bye_msg = Message(MessageType::BYE, {});
  MessageSerialization::encode(bye_msg, encoded_msg);
  if (rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length()) < 0) {
    std::cerr << "Error: Failed to send BYE message\n";
    Close(clientfd);
    return 1;
  }
  
  // Decode the response                                                                                                                                           
  MessageSerialization::decode(response, response_msg);
  if (response_msg.get_message_type() == MessageType::FAILED || response_msg.get_message_type() == MessageType::ERROR) {
    std::cerr << "Error: " << response_msg.get_quoted_text() << std::endl;
    Close(clientfd);
    return 1;
  }
  
  Close(clientfd);
  return 0;
}
