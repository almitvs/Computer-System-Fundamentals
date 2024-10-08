//header file for message functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

enum class MessageType {
  // Used only for uninitialized Message objects
  NONE,

  // Requests
  LOGIN,
  CREATE,
  PUSH,
  POP,
  TOP,
  SET,
  GET,
  ADD,
  SUB,
  MUL,
  DIV,
  BEGIN,
  COMMIT,
  BYE,

  // Responses
  OK,
  FAILED,
  ERROR,
  DATA,
};

class Message {
private:
  //type of message
  MessageType m_message_type;
  //argument
  std::vector<std::string> m_args;

public:
  // Maximum encoded message length (including terminator newline character)
  static const unsigned MAX_ENCODED_LEN = 1024;

  //default constructor
  Message();
  //constructor
  Message( MessageType message_type, std::initializer_list<std::string> args = std::initializer_list<std::string>() );
  //copy constructor
  Message( const Message &other );
  //destructor
  ~Message();
  //assignment operator
  Message &operator=( const Message &rhs );

  //message type
  MessageType get_message_type() const;
  // Setter for the message type
  void set_message_type( MessageType message_type );

  // Retrieve the username argument from the message
  std::string get_username() const;
  std::string get_table() const;
  std::string get_key() const;
  std::string get_value() const;
  std::string get_quoted_text() const;

  //push argument 
  void push_arg( const std::string &arg );

  //check if the message is valid
  bool is_valid() const;

  //get the number of argument
  unsigned get_num_args() const { return m_args.size(); }
  //get the i-th argumnet 
  std::string get_arg( unsigned i ) const { return m_args.at( i ); }
};

#endif // MESSAGE_H
