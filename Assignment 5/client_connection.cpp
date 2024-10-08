//implememtation of connection functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"
#include "value_stack.h"
#include <cstring>
#include <stdexcept>
#include "table.h"
#include <algorithm> 

/*
 * ClientConnection constructor.
 *
 * Parameters:
 *   server - pointer to the Server instance
 *   client_fd - file descriptor for the client connection
 *
 * Initializes the ClientConnection object with the provided server and client file descriptor,
 * and initializes the read buffer.
 */
ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
  , stack()
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}


/*
 * ClientConnection destructor.
 *
 * Closes the client file descriptor when the ClientConnection object is destroyed.
 */
ClientConnection::~ClientConnection()
{
  Close(m_client_fd);
}


/*
 * Handles the communication with the client.
 *
 * Intercepts and processes client messages in a loop until a BYE message or an exception is encountered.
 * Manages transaction states, processes different message types, and handles errors.
 */
void ClientConnection::chat_with_client()
{
    bool transaction_mode = false;
    Message error_msg;
    std::string error_string;
    bool logged_in = false;
    bool has_quit = false;
    // Intercept messages until BYE or an exception
    while (!has_quit) {
      int has_table = 0;
      char encoded_msg[MAXLINE];
      Message decoded_msg;
      try {
      // Error if no message is read
      if (Rio_readlineb(&m_fdbuf, encoded_msg, MAXLINE) <= 0) {
        throw CommException("Failed to intercept client message");
      }
      MessageSerialization::decode(encoded_msg, decoded_msg);
      MessageType request = decoded_msg.get_message_type();
      // Error if first message is not login
      if (!logged_in) {
	if (request == MessageType::LOGIN) {
	  logged_in = true;
	} else {
	  throw InvalidMessage("First message must be login");
	}
      }
      // Process request
	std::string arg1;
	std::string arg2;
	std::string arg3;
	int data1;
        int data2;
	Message data_msg;
	std::string data_string;
	Table* tab;
	switch(request) {
	  //Create case
	case MessageType::CREATE:
	  arg1 = decoded_msg.get_table();
	  if (this->m_server->find_table(arg1) == NULL) {
	    this->m_server->create_table(arg1);
	  } else {
	    throw InvalidMessage("Table with this name already exists");
	  }
	  break;
	  //push case
	case MessageType::PUSH:
	  arg1 = decoded_msg.get_value();
	  this->stack.push(arg1);
	  break;
	  //pop case
	case MessageType::POP:
          this->stack.pop();
	  break;
	  //top case
	case MessageType::TOP:
	  arg1 = this->stack.get_top();
	  data_msg = Message(MessageType::DATA, {arg1});
	  MessageSerialization::encode(data_msg, data_string);
	  if (rio_writen(m_client_fd, data_string.c_str(), data_string.length()) < 0) {
	    throw CommException("Failed to respond to client");
	  }
	  break;
	  //set case
	case MessageType::SET:
	  arg1 = decoded_msg.get_table();
	  arg2 = decoded_msg.get_key();
	  arg3 = this->stack.get_top();
	  this->stack.pop();
	  tab = this->m_server->find_table(arg1);
	  has_table = count(locked_tables.begin(), locked_tables.end(), tab);
	  if (!transaction_mode) {
            tab->lock();
          } else {
	    if (!has_table) { 
	      if (!tab->trylock()) {
		throw FailedTransaction("Failed to acquire lock");
	      }
	      this->locked_tables.push_back(tab);
	    }
          }
	  tab->set(arg2, arg3);
	  if (!transaction_mode) {
	    tab->commit_changes();
	    tab->unlock();
	  } else {
	    this->altered_tables.push_back(tab);
	  }
	  break;
	  //get case
	case MessageType::GET:
	  arg1 = decoded_msg.get_table();
          arg2 = decoded_msg.get_key();
	  tab = this->m_server->find_table(arg1);
	  has_table = count(locked_tables.begin(), locked_tables.end(), tab);
	  if (!transaction_mode) {
            tab->lock();
          } else {
	    if (!has_table) {
	      if (!tab->trylock()) {
		throw FailedTransaction("Failed to acquire lock");
	      }
	      this->locked_tables.push_back(tab);
	    }
	  }
	  arg3 = tab->get(arg2);
	  this->stack.push(arg3);
	  if (!transaction_mode) {
            tab->unlock();
          }
	  break;
	  //add case
	case MessageType::ADD:
	  try {
	    data1 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data2 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data1 += data2;
	    this->stack.push(std::to_string(data1));
	  } catch (const std::invalid_argument& exc5) {
	    throw OperationException("Values must be integers for arithmetic operations");
	  }
	  break;
	  //mul case
	case MessageType::MUL:
	  try {
	    data1 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data2 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data1 *= data2;
	    this->stack.push(std::to_string(data1));
	  } catch (const std::invalid_argument& exc5) {
	    throw OperationException("Values must be integers for arithmetic operations");
	  }
	  break;
	  //sup case
	case MessageType::SUB:
	  try {
	    data1 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data2 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data2 -= data1;
	    this->stack.push(std::to_string(data2));
	  } catch (const std::invalid_argument& exc5) {
	    throw OperationException("Values must be integers for arithmetic operations");
	  }
	  break;
	  //div case
	case MessageType::DIV:
	  try {
	    data1 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data2 = stoi(this->stack.get_top());
	    this->stack.pop();
	    data2 /= data1;
	    this->stack.push(std::to_string(data2));
	  } catch (const std::invalid_argument& exc5) {
	    throw OperationException("Values must be integers for arithmetic operations");
	  }
	  break;
	  //begin message
	case MessageType::BEGIN:
	  if (transaction_mode) {
	    throw FailedTransaction("No nested transactions");
	  }
	  transaction_mode = true;
	  break;
	  //commit message
	case MessageType::COMMIT:
	  this->commit_tables();
	  transaction_mode = false;
	  break;
	  //bye message
	case MessageType::BYE:
	  if (transaction_mode) {
	    this->rollback_tables();
	    transaction_mode = false;
	  }
	  has_quit = true;
	  break;
	  //login messgae
	case MessageType::LOGIN:
	  break;
	default:
	  throw InvalidMessage("Invalid message");
	  break;
	}
	if (request != MessageType::TOP) {
	  Message ok_msg = Message(MessageType::OK, {});
	  std::string ok_string;
	  MessageSerialization::encode(ok_msg, ok_string);	
	  if (rio_writen(m_client_fd, ok_string.c_str(), ok_string.length()) < 0) {
	    throw CommException("Failed to respond to client");
	  }
	}
      }
      //catch case error 1
      catch (const InvalidMessage& exc1) {
	error_msg = Message(MessageType::ERROR, {exc1.what()});
	MessageSerialization::encode(error_msg, error_string);
	rio_writen(m_client_fd, error_string.c_str(), error_string.length());
	this->m_server->log_error(exc1.what());
	if (transaction_mode) {
          this->rollback_tables();
        }
	transaction_mode = false;
	has_quit = true;
      }
      //catch case error 2
      catch (const CommException& exc2) {
	error_msg = Message(MessageType::ERROR, {exc2.what()});
        MessageSerialization::encode(error_msg, error_string);
	rio_writen(m_client_fd, error_string.c_str(), error_string.length());
	this->m_server->log_error(exc2.what());
	if (transaction_mode) {
          this->rollback_tables();
        }
	transaction_mode = false;
	has_quit = true;
      }
      //catch case error 3
      catch (const OperationException& exc3) {
        error_msg = Message(MessageType::FAILED, {exc3.what()});
        MessageSerialization::encode(error_msg, error_string);
        rio_writen(m_client_fd, error_string.c_str(), error_string.length());
        this->m_server->log_error(exc3.what());
	if (transaction_mode) {
	  this->rollback_tables();
	}
	transaction_mode = false;
      }
      //catch case error 4
      catch (const FailedTransaction& exc4) {
        error_msg = Message(MessageType::FAILED, {exc4.what()});
        MessageSerialization::encode(error_msg, error_string);
        rio_writen(m_client_fd, error_string.c_str(), error_string.length());
        this->m_server->log_error(exc4.what());
	this->rollback_tables();
	transaction_mode = false;
      }
   }
}

/*
 * Commits changes to all altered tables and releases their locks.
 *
 * This function is called when a transaction is committed.
 */
void ClientConnection::commit_tables() {
  for (auto it = this->altered_tables.begin(); it != this->altered_tables.end(); ++it) {
    (*it)->commit_changes();
    (*it)->unlock();
  }
  this->altered_tables.clear();
}

/*
 * roll back changes to all altered tables and releases
 *
 * ccalled when transactions fails
 */
void ClientConnection::rollback_tables() {
  for (auto it = this->altered_tables.begin(); it != this->altered_tables.end(); ++it) {
    (*it)->rollback_changes();
    (*it)->unlock();
  }
  this->altered_tables.clear();
}
