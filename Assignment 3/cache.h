//Header file for all the cache functions
//Bob Dong bdong9@jh.edu
//Aidan Alme aalme2@jhu.edu
#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <cstdint>

/*
 * CacheSimulator class represents a cache simulator with configurable
 * parameters for number of sets, blocks per set, block size, and cache policies.
 */
class CacheSimulator {
public:
  /*
   * Constructor to initialize the cache simulator with given parameters.
   *
   * Parameters:
   *   num_sets - Number of sets in the cache
   *   num_blocks - Number of blocks per set
   *   block_size - Size of each block in bytes
   *   write_allocate - Flag to indicate if write-allocate policy is used
   *   write_through - Flag to indicate if write-through policy is used
   *   lru - Flag to indicate if LRU replacement policy is used
   */
  CacheSimulator(uint32_t num_sets, uint32_t num_blocks, uint32_t block_size, bool write_allocate, bool write_through, bool lru);

  /*
   * Perform a read operation on the cache with the given address.
   *
   * Parameters:
   *   address - Memory address to read
   *
   * Returns:
   *   true if the read was a hit, false if it was a miss
   */
  bool read(uint32_t address);

  /*
   * Perform a write operation on the cache with the given address.
   *
   * Parameters:
   *   address - Memory address to write
   *
   * Returns:
   *   true if the write was a hit, false if it was a miss
   */
  bool write(uint32_t address);

  /*
   * Print the statistics of the cache operations.
   * Statistics include total loads, stores, hits, misses, and cycles.
   */
  void print_stats() const;

private:

  //individual block unit
  struct Slot {
    uint32_t tag = 0;
    bool valid = false;
    bool dirty = false;
    uint32_t access_ts = 0;
    uint32_t load_ts = 0;
  };

  //individual cache unit
  struct Set {
    std::vector<Slot> slots;
  };

  //total cache unit
  std::vector<Set> cache;

  //user input
  uint32_t num_sets;
  uint32_t num_blocks;
  uint32_t block_size;
  bool write_allocate;
  bool write_through;
  bool lru;

  //global timestamp for tracking purposes
  uint32_t timestamp;

  //variables to keep track of 
  uint32_t total_loads;
  uint32_t total_stores;
  uint32_t load_hits;
  uint32_t load_misses;
  uint32_t store_hits;
  uint32_t store_misses;
  uint32_t total_cycles;

  /*
   * Calculate the block index from the given memory address.
   *
   * Parameters:
   *   address - Memory address to calculate block index for
   *
   * Returns:
   *   Block index extracted from the address
   */
  uint32_t get_block(uint32_t address) const;

 /*
  * Calculate the tag from the given memory address.
  *
  * Parameters:
  *   address - Memory address to calculate tag for
  *
  * Returns:
  *   Tag extracted from the address
  */
  uint32_t get_tag(uint32_t address) const;

  /*
   * Update the LRU (Least Recently Used) information for the specified block.
   *
   * Parameters:
   *   set_index - Index of the set containing the block
   *   block_index - Index of the block to update
   */
  void update_lru(uint32_t set_index, uint32_t block_index);

  
  /*
   * Update the FIFO (First In, First Out) information for the specified block.
   *
   * Parameters:
   *   set_index - Index of the set containing the block
   *   block_index - Index of the block to update
   */
  void update_fifo(uint32_t set_index, uint32_t block_index);

  /*
   * Evict a block and update the cache with a new block.
   *
   * Parameters:
   *   set_index - Index of the set to evict from
   *   block_index - Index of the block to update
   */
  void evict_and_update(uint32_t set_index, uint32_t block_index);
};

#endif // CACHE_H
