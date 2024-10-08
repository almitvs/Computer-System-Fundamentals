//csim.cpp file for all the implememtation of main and interface functions
//Aidan Alme and Bob Dong
//aalme2@jhu.edu / bdong9@jh.edu

#include <iostream>
#include <sstream>
#include <string>
#include "cache.h"

/*
 * Check if a given number is a power of two.
 *
 * Parameters:
 *   x - The number to check
 *
 * Returns:
 *   true if the number is a power of two, false otherwise
 */
bool is_power_of_two(uint32_t x) {
  return (x != 0) && ((x & (x - 1)) == 0);
}

/*
 * Parse command line arguments to configure the cache simulator.
 *
 * Parameters:
 *   argc - Number of command line arguments
 *   argv - Array of command line arguments
 *   num_sets - Reference to variable for storing number of sets
 *   num_blocks - Reference to variable for storing number of blocks per set
 *   block_size - Reference to variable for storing block size in bytes
 *   write_allocate - Reference to variable for storing write-allocate flag
 *   write_through - Reference to variable for storing write-through flag
 *   lru - Reference to variable for storing LRU replacement policy flag
 *
 * Returns:
 *   true if arguments were successfully parsed, false otherwise
 */
bool parse_arguments(int argc, char* argv[], uint32_t &num_sets, uint32_t &num_blocks, uint32_t &block_size, bool &write_allocate, bool &write_through, bool &lru) {
  if (argc != 7) {
    std::cerr << "Usage: " << argv[0] << " <num_sets> <num_blocks> <block_size> <write-allocate|no-write-allocate> <write-through|write-back> <lru|fifo>" << std::endl;
    return false;
  }
  
  num_sets = std::stoul(argv[1]);
  num_blocks = std::stoul(argv[2]);
  block_size = std::stoul(argv[3]);
  
  if (!is_power_of_two(num_sets) || !is_power_of_two(num_blocks) || !is_power_of_two(block_size) || block_size < 4) {
    std::cerr << "Invalid cache configuration parameters!" << std::endl;
    return false;
  }
  
  std::string write_allocate_str = argv[4];
  write_allocate = (write_allocate_str == "write-allocate");
  if (write_allocate_str != "write-allocate" && write_allocate_str != "no-write-allocate") {
    std::cerr << "Invalid write-allocate parameter!" << std::endl;
    return false;
  }
  
  std::string write_through_str = argv[5];
  write_through = (write_through_str == "write-through");
  if (write_through_str != "write-through" && write_through_str != "write-back") {
    std::cerr << "Invalid write-through parameter!" << std::endl;
    return false;
  }
  
  if (!write_allocate && !write_through) {
    std::cerr << "Invalid combination: no-write-allocate and write-back!" << std::endl;
    return false;
    }
  
  std::string lru_str = argv[6];
  lru = (lru_str == "lru");
  if (lru_str != "lru" && lru_str != "fifo") {
    std::cerr << "Invalid eviction policy parameter!" << std::endl;
    return false;
  }
  
  return true;
}

/*
 * Process a trace file and simulate cache operations.
 *
 * Parameters:
 *   simulator - Reference to the CacheSimulator object
 *   trace - Input stream for reading the trace file
 */
void process_trace(CacheSimulator &simulator, std::istream &trace) {
  std::string line;
  while (std::getline(trace, line)) {
    std::istringstream iss(line);
    char op;
    uint32_t address;
    int size;
    iss >> op >> std::hex >> address >> size;
    
    if (op == 'l') {
      simulator.read(address);
    } else if (op == 's') {
      simulator.write(address);
    }
  }
}

/*
 * Main function to initialize the cache simulator and process the trace file.
 *
 * Parameters:
 *   argc - Number of command line arguments
 *   argv - Array of command line arguments
 *
 * Returns:
 *   Exit status of the program
 */
int main(int argc, char* argv[]) {
  uint32_t num_sets, num_blocks, block_size;
  bool write_allocate, write_through, lru;
  
  if (!parse_arguments(argc, argv, num_sets, num_blocks, block_size, write_allocate, write_through, lru)) {
    return 1;
  }
  
  CacheSimulator simulator(num_sets, num_blocks, block_size, write_allocate, write_through, lru);
  
  process_trace(simulator, std::cin);
  
  simulator.print_stats();
  
  return 0;
}
