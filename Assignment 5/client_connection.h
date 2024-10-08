//header file for client connection functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"
#include "value_stack.h"
#include <vector>

class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server;
  int m_client_fd;
  rio_t m_fdbuf;
  ValueStack stack;
  //variabls for mutex
  std::vector<Table*> locked_tables;
  std::vector<Table*> altered_tables;
  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );
  //commit table if success
  void commit_tables();
  //rollback table if failed
  void rollback_tables();

public:
  //constructor
  ClientConnection( Server *server, int client_fd );
  //destructor
  ~ClientConnection();
  //chat function
  void chat_with_client();
  };

#endif // CLIENT_CONNECTION_H
