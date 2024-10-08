//.cpp file for all the implememtation of cache functions                                
//Aidan Alme and Bob Dong                                                                                
//aalme2@jhu.edu / bdong9@jh.edu 
#include <vector>
#include <cstdint>
#include <iostream>
#include "cache.h"
#include <cmath>
#include <climits>


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
CacheSimulator::CacheSimulator(uint32_t num_sets, uint32_t num_blocks, uint32_t block_size, bool write_allocate, bool write_through, bool lru)
  : num_sets(num_sets), num_blocks(num_blocks), block_size(block_size), write_allocate(write_allocate), write_through(write_through), lru(lru),
    timestamp(0), total_loads(0), total_stores(0), load_hits(0), load_misses(0), store_hits(0), store_misses(0), total_cycles(0) {
  cache.resize(num_sets);
  for (uint32_t i = 0; i < num_sets; ++i) {
    cache[i].slots.resize(num_blocks);
  }
}


/*
 * Calculate the block index from the given memory address.
 *
 * Parameters:
 *   address - Memory address to calculate block index for
 *
 * Returns:
 *   Block index extracted from the address
 */
uint32_t CacheSimulator::get_block(uint32_t address) const {
  // |<----Tag----->|<-------index(block)-------|<------Offset----->|
  return (address / block_size) % num_sets;
}

/*
 * Calculate the tag from the given memory address.
 *
 * Parameters:
 *   address - Memory address to calculate tag for
 *
 * Returns:
 *   Tag extracted from the address
 */
uint32_t CacheSimulator::get_tag(uint32_t address) const {
   // |<----Tag----->|<-------index(block)-------|<------Offset----->|  
  return (address / block_size) / num_sets;
}

/*
 * Update the LRU (Least Recently Used) information for the specified block.
 *
 * Parameters:
 *   set_index - Index of the set containing the block
 *   block_index - Index of the block to update
 */
void CacheSimulator::update_lru(uint32_t set_index, uint32_t block_index) {
  //access time stamp updated
  cache[set_index].slots[block_index].access_ts = ++timestamp;
}

 /*
  * Update the FIFO (First In, First Out) information for the specified block.
  *
  * Parameters:
  *   set_index - Index of the set containing the block
  *   block_index - Index of the block to update
  */
void CacheSimulator::update_fifo(uint32_t set_index, uint32_t block_index) {
  //load_time update
  cache[set_index].slots[block_index].load_ts = ++timestamp;
}

 /*
  * Evict a block and update the cache with a new block.
  *
  * Parameters:
  *   set_index - Index of the set to evict from
  *   block_index - Index of the block to update
  */
void CacheSimulator::evict_and_update(uint32_t set_index, uint32_t block_index) {
  Slot &slot = cache[set_index].slots[block_index];

  //if the slot is a dirty bit (has information in it) and is write_back
  if (slot.dirty && !write_through) {
    //loads/stores from/to memory take 100 processor cycles for each 4-byte quantity that is transferred
    total_cycles += 100 * (block_size / 4); // Write-back dirty block
  }

  //set the bit to be empty and clean
  slot.valid = false;
  slot.dirty = false;
}


/*
 * Perform a read operation on the cache with the given address.
 *
 * Parameters:
 *   address - Memory address to read
 *
 * Returns:
 *   true if the read was a hit, false if it was a miss
 */
bool CacheSimulator::read(uint32_t address) {
  ++total_loads;
  uint32_t set_index = get_block(address);
  uint32_t tag = get_tag(address);
  Set &set = cache[set_index];
  //load hit
  for (uint32_t i = 0; i < num_blocks; ++i) {
    if (set.slots[i].valid && set.slots[i].tag == tag) {
      ++load_hits;
      //load hit add 1 cycle
      total_cycles += 1; 
      if (lru) {
	update_lru(set_index, i);
      } //no need to update time stamp for fifo as it is already in cache
      return true;
    }
  }

  //load miss 
  ++load_misses;
  // Load miss takes 100 cycles per block_size bytes
  total_cycles += 100 * (block_size / 4); 
  // Load in the data from memory
    //load_index
    int lru_index = -1;
    uint32_t min_ts = UINT32_MAX;
    //if the block is empty, set the load_index to it 
    for (uint32_t i = 0; i < num_blocks; ++i) {
      if (!set.slots[i].valid) {
	lru_index = i;
	break;
      }
      //else compare access_ts for lru, the smaller the ts, the older the last access time
      if (lru && set.slots[i].access_ts < min_ts) {
	min_ts = set.slots[i].access_ts;
	lru_index = i;
	//else check load_ts for lru, as fifo chuck the older slot
      } else if (!lru && set.slots[i].load_ts < min_ts) {
	min_ts = set.slots[i].load_ts;
	lru_index = i;
      }
    }
    //empty and update the dirty block 
    if (set.slots[lru_index].valid) {
      evict_and_update(set_index, lru_index);
    }
    set.slots[lru_index].tag = tag;
    set.slots[lru_index].valid = true;
    //update lru
    if (lru) {
      update_lru(set_index, lru_index);
      //update fifo
    } else {
      update_fifo(set_index, lru_index);
    }
  // Load the data
  total_cycles++;
  
  // return false for load miss
  return false;
}


/*
 * Perform a write operation on the cache with the given address.
 *
 * Parameters:
 *   address - Memory address to write
 *
 * Returns:
 *   true if the write was a hit, false if it was a miss
 */


bool CacheSimulator::write(uint32_t address) {
  ++total_stores;
  uint32_t set_index = get_block(address);
  uint32_t tag = get_tag(address);
  Set &set = cache[set_index];

  //iterate through every block in a set
  for (uint32_t i = 0; i < num_blocks; ++i) {
    
    //hit (tag matches and slot is valid)
    if (set.slots[i].valid && set.slots[i].tag == tag) {
      ++store_hits;
      total_cycles += 1; // Store hit takes 1 cycle
      if (write_through) {
	// Write-through to memory  
	total_cycles += 100;
	//write-allocate
      } else {
	//make the slot dirty and take care later  
	set.slots[i].dirty = true;
      }
      //update lru
      if (lru) {
	update_lru(set_index, i);
	//update fifo
      } else {
	//update_fifo(set_index, i); not needed because info already in slot
      }
      //write successful
      return true; 
    }
  }

  //if not store hit, store miss
  ++store_misses;
  if (write_allocate) {
    // Write miss takes 100 cycles per block_size bytes 
    total_cycles += 100 * (block_size / 4); 
    //load index
    int lru_index = -1;
    //initialize variable
    uint32_t min_ts = UINT32_MAX;
    for (uint32_t i = 0; i < num_blocks; ++i) {
      //if we find a empty slot (not valid), load_index = slot index
      if (!set.slots[i].valid) {
	lru_index = i;
	break;
      }
      //if lru and access_ts is smaller than min_ts
      if (lru && set.slots[i].access_ts < min_ts) {
	//set load_index to current slot and update min_ts  
	min_ts = set.slots[i].access_ts;
	lru_index = i;
	//similar thing for fifo
      } else if (!lru && set.slots[i].load_ts < min_ts) {
	min_ts = set.slots[i].load_ts;
	lru_index = i;
      }
    }
    //update the slot with our data
    if (set.slots[lru_index].valid) {
      evict_and_update(set_index, lru_index);
      }
    set.slots[lru_index].tag = tag;
    set.slots[lru_index].valid = true;
    set.slots[lru_index].dirty = !write_through;
    // Write through to cache and memory
    if (write_through) {
      total_cycles++;
      total_cycles += 100;
    }
    //update lru
    if (lru) {
      update_lru(set_index, lru_index);
      //update fifo
    } else {
      update_fifo(set_index, lru_index);
    }
    // No-write-allocate, write directly to memory  
  } else {
    total_cycles += 100; 
  }
  return false;
}


/*
 * Print the statistics of the cache operations.
 * Statistics include total loads, stores, hits, misses, and cycles.
 */
void CacheSimulator::print_stats() const {
  std::cout << "Total loads: " << total_loads << std::endl;
  std::cout << "Total stores: " << total_stores << std::endl;
  std::cout << "Load hits: " << load_hits << std::endl;
  std::cout << "Load misses: " << load_misses << std::endl;
  std::cout << "Store hits: " << store_hits << std::endl;
  std::cout << "Store misses: " << store_misses << std::endl;
  std::cout << "Total cycles: " << total_cycles << std::endl;
}
