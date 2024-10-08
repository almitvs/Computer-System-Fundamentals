//header file for server function
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <string>
#include <pthread.h>
#include "table.h"
#include "client_connection.h"

class Server {
private:
  //variables and private functions
  int serverfd;
  std::map<std::string, Table*> tables;
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  //functions for connection
  void listen( const std::string &port );
  void server_loop();

  //working functions
  static void *client_worker( void *arg );

  //error function
  void log_error( const std::string &what );


  //concurrency function
  void create_table( const std::string &name );
  
  Table *find_table( const std::string &name );
};


#endif // SERVER_H
