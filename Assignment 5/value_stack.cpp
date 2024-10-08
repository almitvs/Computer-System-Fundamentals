//implememtation of stack functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include "value_stack.h"
#include "exceptions.h"

#include "value_stack.h"
#include "exceptions.h"

/*
 * Constructor for the ValueStack class.
 * Initializes an empty value stack.
 */
ValueStack::ValueStack()
{
}

/*
 * Destructor for the ValueStack class.
 * Cleans up any resources used by the value stack.
 */
ValueStack::~ValueStack()
{
}

/*
 * Checks if the value stack is empty.
 *
 * Returns:
 *   True if the stack is empty, otherwise false
 */
bool ValueStack::is_empty() const
{
  return this->stack.empty();
}

/*
 * Pushes a value onto the value stack.
 *
 * Parameters:
 *   value - The value to push onto the stack
 */
void ValueStack::push( const std::string &value )
{
  this->stack.push_back(value);
}

/*
 * Retrieves the value at the top of the value stack.
 *
 * Returns:
 *   The value at the top of the stack
 *
 * Throws:
 *   OperationException if the stack is empty
 */
std::string ValueStack::get_top() const
{
  if (this->is_empty()) {
    throw OperationException("ValueStack is empty");
  }
  return this->stack.back();
}

/*
 * Pops the value at the top of the value stack.
 *
 * Throws:
 *   OperationException if the stack is empty
 */
void ValueStack::pop()
{
  if (this->is_empty()) {
    throw OperationException("ValueStack is empty");
  }
  this->stack.pop_back();
}
