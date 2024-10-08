//implementation of set value client
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include "csapp.h"
#include <iostream>
#include "message.h"
#include "message_serialization.h"
#include <cstring>
#include <set>


int main(int argc, char **argv) {
    if (argc != 7) {
        std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
        return 1;
    }

    std::string hostname = argv[1];
    std::string port = argv[2];
    std::string username = argv[3];
    std::string table = argv[4];
    std::string key = argv[5];
    std::string value = argv[6];

    int clientfd = open_clientfd(hostname.c_str(), port.c_str());
    if (clientfd < 0) {
        std::cerr << "Failed to connect to server.\n";
        return 1;
    }

    //read buffer
    rio_t rio;
    //read into read buffer
    Rio_readinitb(&rio, clientfd);

    //Send LOGIN message
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

    // Send PUSH message
    Message push_msg = Message(MessageType::PUSH, {value});
    MessageSerialization::encode(push_msg, encoded_msg);
    if (rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length()) < 0) {
        std::cerr << "Error: Failed to send GET message\n";
        Close(clientfd);
        return 1;
    }

    // Read PUSH response
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
    
    // Send SET message
    Message set_msg = Message(MessageType::SET, {table, key});
    MessageSerialization::encode(set_msg, encoded_msg);
    if (rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length()) < 0) {
      std::cerr << "Error: Failed to send GET message\n";
      Close(clientfd);
      return 1;
    }
    
    // Read SET response
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
