//implementation of incre value function
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include "csapp.h"
#include <iostream>
#include <sstream>
#include "message.h"
#include "message_serialization.h"
#include <cstring>
#include <set>

int main(int argc, char **argv) {
    if (argc != 6 && (argc != 7 || std::string(argv[1]) != "-t")) {
        std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
        std::cerr << "Options:\n";
        std::cerr << "  -t      execute the increment as a transaction\n";
        return 1;
    }

    int count = 1;
    bool use_transaction = false;

    if (argc == 7) {
        use_transaction = true;
        count = 2;
    }

    std::string hostname = argv[count++];
    std::string port = argv[count++];
    std::string username = argv[count++];
    std::string table = argv[count++];
    std::string key = argv[count++];

    int clientfd = open_clientfd(hostname.c_str(), port.c_str());
    if (clientfd < 0) {
        std::cerr << "Error: Failed to connect to server (" << hostname << ":" << port << ").\n";
        return 1;
    }

    rio_t rio;
    Rio_readinitb(&rio, clientfd);

    // Send LOGIN message
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

    if (use_transaction) {
      // Send BEGIN message                                                                                                                                                                                  
      Message begin_msg = Message(MessageType::BEGIN, {});
      MessageSerialization::encode(begin_msg, encoded_msg);
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


    // Send PUSH message
    Message push_msg = Message(MessageType::PUSH, {"1"});
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

    // Send ADD message
    Message add_msg = Message(MessageType::ADD, {});
    MessageSerialization::encode(add_msg, encoded_msg);
    if (rio_writen(clientfd, encoded_msg.c_str(), encoded_msg.length()) < 0) {
      std::cerr << "Error: Failed to send GET message\n";
      Close(clientfd);
      return 1;
    }
    
    // Read ADD response
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

    if (use_transaction) {
      // Send COMMIT message                                                                                                                                                                                   
      Message commit_msg = Message(MessageType::COMMIT, {});
      MessageSerialization::encode(commit_msg, encoded_msg);
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
