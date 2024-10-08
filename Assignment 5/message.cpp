//implememtation of message functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"

/*
 * Default constructor initializing an empty message.
 *
 * Sets the message type to NONE.
 */
Message::Message()
  : m_message_type(MessageType::NONE)
{
}
/*
 * Constructor initializing a message with a specified type and arguments.
 *
 * Parameters:
 *   message_type - The type of the message
 *   args - Initializer list of arguments associated with the message
 */
Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}
/*
 * Copy constructor.
 *
 * Parameters:
 *   other - The Message object to copy from
 */
Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}
/*
 * Destructor.
 */
Message::~Message()
{
}
/*
 * Assignment operator.
 *
 * Parameters:
 *   rhs - The right-hand side Message object to assign from
 *
 * Returns:
 *   A reference to the assigned Message object
 */
Message &Message::operator=( const Message &rhs )
{
  this->m_message_type = rhs.m_message_type;
  this->m_args = rhs.m_args;
  return *this;
}
/*
 * Getter for the message type.
 *
 * Returns:
 *   The type of the message
 */
MessageType Message::get_message_type() const
{
  return m_message_type;
}
/*
 * Setter for the message type.
 *
 * Parameters:
 *   message_type - The new type of the message
 */
void Message::set_message_type(MessageType message_type)
{
  m_message_type = message_type;
}
/*
 * Retrieve the username argument from the message.
 *
 * Returns:
 *   The username if the message type is LOGIN, otherwise an empty string
 */
std::string Message::get_username() const
{
  if (this->m_message_type == MessageType::LOGIN) {
    return this->m_args[0];
  } else {
    return "";
  }
}
/*
 * Retrieve the table name argument from the message.
 *
 * Returns:
 *   The table name if the message type is CREATE, SET, or GET, otherwise an empty string
 */
std::string Message::get_table() const
{
  if (this->m_message_type == MessageType::CREATE || this->m_message_type == MessageType::SET || this->m_message_type == MessageType::GET) {
    return this->m_args[0];
  } else {
    return "";
  }
}
/*
 * Retrieve the key argument from the message.
 *
 * Returns:
 *   The key if the message type is SET or GET, otherwise an empty string
 */
std::string Message::get_key() const
{
  if (this->m_message_type == MessageType::SET || this->m_message_type == MessageType::GET) {
    return this->m_args[1];
  } else {
    return "";
  }
}
/*
 * Retrieve the value argument from the message.
 *
 * Returns:
 *   The value if the message type is PUSH or DATA, otherwise an empty string
 */
std::string Message::get_value() const
{
  if (this->m_message_type == MessageType::PUSH || this->m_message_type == MessageType::DATA) {
    return this->m_args[0];
  } else {
    return "";
  }
}
/*
 * Retrieve the quoted text argument from the message.
 *
 * Returns:
 *   The quoted text if the message type is FAILED or ERROR, otherwise an empty string
 */
std::string Message::get_quoted_text() const
{
  if (this->m_message_type == MessageType::FAILED || this->m_message_type == MessageType::ERROR) {
    return this->m_args[0];
  } else {
    return "";
  }
}
/*
 * Add an argument to the message.
 *
 * Parameters:
 *   arg - The argument to add to the message
 */
void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

/*
 * Check if the message is valid.
 *
 * Returns:
 *   True if the message is valid, otherwise false
 */
bool Message::is_valid() const
{
  std::regex identifier("^[a-zA-Z][a-zA-Z0-9_]*$");
  std::regex value("^\\S+$");
  std::regex quoted_text("^[^\"]*$");
  
  switch(this->m_message_type) {
  case MessageType::LOGIN:
  case MessageType::CREATE:
    //check for argument size
    if (this->m_args.size() != 1) return false;
    return regex_match(this->m_args[0], identifier);
    break;
  case MessageType::PUSH:
  case MessageType::DATA:
    //check for argument size
    if (this->m_args.size() != 1) return false;
    return regex_match(this->m_args[0], value);
    break;
  case MessageType::SET:
  case MessageType::GET:
    if (this->m_args.size() != 2) return false;
    return regex_match(this->m_args[0], identifier) && regex_match(this->m_args[1], identifier);
    break;
  case MessageType::POP:
  case MessageType::TOP:
  case MessageType::ADD:
  case MessageType::SUB:
  case MessageType::MUL:
  case MessageType::DIV:
  case MessageType::BEGIN:
  case MessageType::COMMIT:
  case MessageType::BYE:
  case MessageType::OK:
    if (this->m_args.size() != 0) return false;
    return true;
    break;
  case MessageType::FAILED:
  case MessageType::ERROR:
    if (this->m_args.size() != 1) return false;
    return regex_match(this->m_args[0], quoted_text);
    break;
  default:
    return true;
  }
}
