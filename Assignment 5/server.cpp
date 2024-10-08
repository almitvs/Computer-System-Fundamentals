//implememtation of server functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"
#include "client_connection.h"
#include <pthread.h>
#include "table.h"
#include <cstring>


/*
 * Default constructor for the Server class.
 */
Server::Server()
{
}

/*
 * Destructor for the Server class. Cleans up allocated resources.
 * Deletes all Table objects in the tables map and closes the server socket if it's open.
 */
Server::~Server()
{
  //iterate over the table
  for (auto it = tables.begin(); it != tables.end(); ++it) {
    delete it->second;
  }
  //close if the socket is opened
  if (serverfd >= 0) {
    close(serverfd);
  }
}

/*
 * Binds and listens on a specified port.
 * 
 * Parameters:
 *   port - the port number to listen on
 * 
 * Throws:
 *   CommException if unable to listen on the server socket
 */
void Server::listen( const std::string &port )
{
  //open socket on the port
  this->serverfd = open_listenfd(port.c_str());
  if (this->serverfd < 0) {
    throw CommException("Could not listen on server socket");
  }
}

/*
 * Main server loop that accepts incoming client connections
 * and spawns a new thread to handle each client.
 */
void Server::server_loop()
{
  while (true) {
    //thread connection
    int clientfd = Accept(serverfd, NULL, NULL);
    //create client connection object
    ClientConnection *client = new ClientConnection( this, clientfd );
    pthread_t thr_id;
    //connection
    if ( pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ) {
      throw CommException( "Could not create client thread" );
    }
    //detach the thread
    pthread_detach(thr_id);
  }
}

/*
 * Thread worker function that processes client requests.
 * 
 * Parameters:
 *   arg - pointer to the ClientConnection object
 * 
 * Returns:
 *   nullptr
 */
void *Server::client_worker( void *arg )
{
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;
}

/*
 * Logs an error message to the standard error output.
 * 
 * Parameters:
 *   what - the error message to log
 */
void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

/*
 * Creates a new Table object and adds it to the tables map.
 * 
 * Parameters:
 *   name - the name of the table
 */

void Server::create_table( const std::string &name )
{
  this->tables[name] = new Table(name);
}

/*
 * Finds a Table object by name in the tables map.
 * 
 * Parameters:
 *   name - the name of the table to find
 * 
 * Returns:
 *   A pointer to the Table object if found, or nullptr if not found
 */
Table* Server::find_table( const std::string &name )
{ Table* table = nullptr;
  //iterate through table
  auto it = tables.find(name);
  if (it != tables.end())
    {
      table = it->second;
    }
  return table;
}
