//implememtation of table functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu
#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

/*
 * Constructor initializing a table with a specified name.
 * Initializes the mutex for the table.
 *
 * Parameters:
 *   name - The name of the table
 */
Table::Table( const std::string &name )
  : m_name( name )
{
  pthread_mutex_init(&this->mutex, NULL);
}

/*
 * Destructor.
 * Destroys the mutex associated with the table.
 */
Table::~Table()
{
  pthread_mutex_destroy(&this->mutex);
}

/*
 * Locks the table for exclusive access.
 */
void Table::lock()
{
  pthread_mutex_lock(&this->mutex);
}

/*
 * Unlocks the table.
 */
void Table::unlock()
{
  pthread_mutex_unlock(&this->mutex);
}

/*
 * Tries to lock the table for exclusive access.
 *
 * Returns:
 *   True if the lock was acquired, otherwise false
 */
bool Table::trylock()
{
  return pthread_mutex_trylock(&this->mutex) == 0;
}

/*
 * Sets a key-value pair in the tentative table.
 *
 * Parameters:
 *   key - The key to set
 *   value - The value to associate with the key
 */
void Table::set( const std::string &key, const std::string &value )
{
  this->tentative_table[key] = value;
}

/*
 * Retrieves the value associated with a key from the tentative table.
 *
 * Parameters:
 *   key - The key to retrieve the value for
 *
 * Returns:
 *   The value associated with the key
 */
std::string Table::get( const std::string &key )
{
  std::string value = this->tentative_table[key];
  return value;
}

/*
 * Checks if the tentative table contains a key.
 *
 * Parameters:
 *   key - The key to check for
 *
 * Returns:
 *   True if the key is present, otherwise false
 */
bool Table::has_key( const std::string &key )
{
  for (auto it = this->tentative_table.cbegin(); it != this->tentative_table.cend(); ++it) {
    if (it->first == key) {
      return true;
    }
  }
  return false;
}

/*
 * Commits the changes from the tentative table to the main table.
 */
void Table::commit_changes()
{
  this->table = this->tentative_table;
}

/*
 * Rolls back the changes, reverting the tentative table to the main table.
 */
void Table::rollback_changes()
{
  this->tentative_table = this->table;
}
