#include <cassert>
#include "bigint.h"
#include <sstream>
#include <iomanip>
#include <iostream>

//default constructor
BigInt::BigInt() : bits(1, 0), negative(false) {}

// constructor with single unit64_t value and a boolean expression  
BigInt::BigInt(uint64_t val, bool negative) : bits(1, val), negative(negative) {}

// constructor with initializer_list of values of and  boolean expression
BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative) : bits(vals), negative(negative) {}

// constructor that accept an vector
BigInt::BigInt(std::vector<uint64_t> vals, bool negative) : bits(vals), negative(negative) {}

// copy constructor
BigInt::BigInt(const BigInt &other) : bits(other.bits), negative(other.negative) {}

//don't really need to do anything for destructor
BigInt::~BigInt(){}

//assignment operator
BigInt &BigInt::operator=(const BigInt &rhs) {
  //make sure rhs is not the current object
  if (this != &rhs) {
    bits = rhs.bits;
    negative = rhs.negative;
  }

  //return a reference to the current object
  return *this;
}

//return the status of negativity of the object
bool BigInt::is_negative() const {
  return negative;
}

//return a constant reference to the bit vector
const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  return bits;
}

//return the uint64_t value of the index at the bit vector
uint64_t BigInt::get_bits(unsigned index) const {
  //check to see if idnex is out of bound
  if (index < bits.size()) {
    
    return bits[index];
  } else {
    return 0;
  }
}

//find the lead bit of the BigINt
int BigInt::find_lead_bit(const BigInt &num) const {
  int index = 0;

  //iterate through the number
  for (auto i = num.bits.size() - 1; i > 0; i--) {
    if (num.bits[i] != 0) {
      index = i;
      break;
    }
  }

  //return result
  return index;
}

//private helper function to compute the sum of BigInts with the same sign
BigInt BigInt::add(const BigInt &lhs, const BigInt &rhs) const {

  //initialize varibales and split the number
  BigInt sum = BigInt({}, false);
  uint64_t overflow = 0UL;
  int left_lead = find_lead_bit(lhs);
  int right_lead = find_lead_bit(rhs);
  int lead = (left_lead > right_lead) ? left_lead : right_lead;

  //iterate through the number 
  for (int i = 0; i <= lead; i++) {
    uint64_t left;
    try {
      left = lhs.bits.at(i);

      //catch errors
    } catch (std::out_of_range const& exc) {
      left = 0UL;
    }
    uint64_t right;
    try	{
      right = rhs.bits.at(i);

      //catch errors
    } catch (std::out_of_range const& exc) {
      right = 0UL;
    }

    //update overflow
    uint64_t new_overflow = 0UL;
    uint64_t digit_sum = left + right;
    if (digit_sum < left) {
      new_overflow++;
      digit_sum += overflow;
    } else {
      digit_sum += overflow;
      if (digit_sum < overflow) {
	new_overflow++;
      }
    }

    //get the bits into the sum
    sum.bits.push_back(digit_sum);
    overflow = new_overflow;
  }
  if (overflow != 0UL) {
    sum.bits.push_back(overflow);
  }

  //return results
  return sum;
}

//private helper function to find whether the absolute value of a BigInt is greater than another or not
bool BigInt::greater_magnitude(const BigInt &lhs, const BigInt &rhs) const {

  //initilaize values
  int left_lead = find_lead_bit(lhs);
  int right_lead = find_lead_bit(rhs);

  //direct comparision
  if (left_lead > right_lead) {
    return true;
  } else if (right_lead > left_lead) {
    return false;
  } else {
    for (int i = left_lead; i >= 0; i--) {
      if (lhs.bits[i] > rhs.bits[i]) {
	return true;
      } else if (lhs.bits[i] < rhs.bits[i]) {
	return false;
      }
    }

    //return false if nothing matched
    return false;
  }
}

//private helper function to compute the addition of mixed sign BigInts
BigInt BigInt::subtract(const BigInt &lhs, const BigInt &rhs) const {

  //initialize variables
  BigInt dif = BigInt({}, false);
  uint64_t overflow = 0UL;
  int lead = find_lead_bit(lhs);
  for (int i = 0; i <= lead; i++) {
    uint64_t left = lhs.bits.at(i);
    uint64_t right;
    
    try {
      right = rhs.bits.at(i);

      //catch errors
    } catch (std::out_of_range const& exc) {
      right = 0UL;
    }

    //update overflow
    uint64_t new_overflow = 0UL;
    uint64_t digit_dif = left - right;
    if (digit_dif > left) {
      new_overflow++;
      digit_dif -= overflow;
    } else {
      uint64_t new_digit_dif = digit_dif - overflow;
      if (new_digit_dif > digit_dif) {
        new_overflow++;
      }
      digit_dif = new_digit_dif;
    }

    //add results
    dif.bits.push_back(digit_dif);
    overflow = new_overflow;
  }

  //return results
  return dif;
}

//add Bigint numbers
BigInt BigInt::operator+(const BigInt &rhs) const {
  BigInt sum;

  //compare signs
  if (this->negative == rhs.negative) {
    sum = add(rhs, *this);
    sum.negative = this->negative;
  } else {
    if (greater_magnitude(*this, rhs)) {
      sum = subtract(*this, rhs);
      sum.negative = this->negative;
    } else {
      sum = subtract(rhs, *this);
      sum.negative = rhs.negative;
    }
  }

  //special case
  if (sum.is_zero()) {
    return BigInt(0);
  }

  //return results
  return sum;
}

//subtract BigInt numbers
BigInt BigInt::operator-(const BigInt &rhs) const {
  BigInt neg = BigInt(rhs);
  neg.negative = !rhs.negative;
  return *this + neg;
}

//unary minus
BigInt BigInt::operator-() const {
  //copy this object
  BigInt new_BigInt(*this);

  bool isZero = true;
  
  for (const auto& bit : bits)
    {
      if (bit != 0) {
	isZero = false;
      }
    }

  if (!isZero) {
    //negate this object
    new_BigInt.negative = !negative;
  }
  
  return new_BigInt;
}

// Test whether a specific bit in the bit string is set to 1
bool BigInt::is_bit_set(unsigned n) const {

  //check the index of the bit
  auto index = n / 64;

  //chexk the position of the bit inside the index
  auto position = n % 64;

  //if the bit is out of range, return false
  if (index >= bits.size()) {
    return false;
  }

  //compare the bit with 1
  return (bits[index] >> position) & 1;
}

//left shift by n bits
BigInt BigInt::operator<<(unsigned n) const {

  //check if negative
  if (negative) {
    throw std::invalid_argument("BigInt is negative.");
  }

  //copy this for operation
  BigInt result(*this);

  //calculate the shifts across words and shifts within words
  unsigned fullShifts = n / 64;
  unsigned bitShift = n % 64;

  //shift across the words first
  if (fullShifts > 0) {
    result.bits.insert(result.bits.begin(), fullShifts, 0);
  }

  uint64_t carry = 0;

  //starting from originally where the shift begin
  for (auto i = fullShifts; i < result.bits.size(); ++ i) {

    //calculate newCarry
    uint64_t newCarry = (bitShift == 0) ? 0 : (result.bits[i] >> (64 - bitShift));

    //commit the shift within the word
    result.bits[i] = (result.bits[i] << bitShift) | carry;

    //update carry
    carry = newCarry;
  }

  //add the carry if carry not resolved
  if (carry != 0) {
    result.bits.push_back(carry);
  }

  return result;
  
}

//multiply BigInt numbers
BigInt BigInt::operator*(const BigInt &rhs) const {
  if (this->is_zero() || rhs.is_zero()) {
    return BigInt(0UL);
  }

  //initialize variables
  BigInt left = BigInt(*this);
  BigInt right = BigInt(rhs);
  left.negative = false;
  right.negative = false;
  std::vector<unsigned> twos;

  //iterate through left bits
  for (auto i = 0; i < (int)(left.bits.size() * 64UL); i++) {
    if (left.is_bit_set(i)) {
      twos.push_back(i);
    }
  }

  BigInt shifter = BigInt(right);
  std::vector<BigInt> products;

  //iterate through twos
  for (auto it = twos.cbegin(); it != twos.cend(); ++it) {
    products.push_back(shifter << *it);
  }
  BigInt product;

  //update product
  for (auto it = products.cbegin(); it != products.cend(); ++it) {
    product = product + *it;
  }

  //calculate signs
  if (this->negative == rhs.negative) {
    product.negative = false;
  } else {
    product.negative = true;
  }
  
  return product;
}

//private helper function to determine if a BigInt is zero
bool BigInt::is_zero() const {
  for (auto it = this->bits.cbegin(); it != this->bits.cend(); ++it) {
    if (*it != 0UL) {
      return false;
    }
  }
  return true;
}

//private helper function to divide by two
BigInt BigInt::divide_by_2() const {
  if (this->is_zero()) {
    return BigInt(0UL);
  }
  BigInt quotient = BigInt(*this);
  int index = find_lead_bit(quotient);
  for (int i = 0; i < index; i++) {
    quotient.bits[i] = (quotient.bits[i] >> 1);
    if (quotient.bits[i + 1] & 1UL) {
      quotient.bits[i] += 9223372036854775808UL;
    }
  }
  quotient.bits[index] = (quotient.bits[index] >> 1);
  quotient.negative = this->negative;
  return quotient;
}


BigInt BigInt::operator/(const BigInt &rhs) const {

  //check for zero divisor
  if (rhs.is_zero()) {
    throw std::invalid_argument("Divide by zero error");
  }

  //return zero for zero dividend
  if (this->is_zero()) {
    return BigInt(0UL);
  }

  //return 1 if equal
  if (this->compare(rhs) == 0) {
    return BigInt(1U);
  }

  //initialize variables
  BigInt left = BigInt(0);
  BigInt right = BigInt(*this);
  BigInt denom = rhs;
  BigInt num = *this;

  denom.negative = false;
  num.negative = false;
  right.negative = false;

  if (num.compare(denom) < 0) {
    return BigInt(0UL);
  }

  //binary operations
  while (left.compare(right) <= 0) {
    BigInt mid = (left + right).divide_by_2();
    BigInt mid_mult_denom = mid * denom;
    BigInt mid_plus_one_mult_denom = (mid + BigInt(1UL)) * denom;

    //check if mid* denom is less than or equal to num 
    if (mid_mult_denom.compare(num) <= 0 && mid_plus_one_mult_denom.compare(num) > 0) {
      BigInt result = mid;
      result.negative = this->negative != rhs.negative;
      return result;
    } else if (mid_mult_denom.compare(num) < 0) {
      left = mid + BigInt(1UL);
    } else {
      right = mid - BigInt(1UL);
    }
  }

  //get quotient
  return BigInt(0UL);
}




//comapre the value of a BigInt to another
int BigInt::compare(const BigInt &rhs) const {

  //compare negativity
  if (negative && !rhs.negative) return -1;
  if (!negative && rhs.negative) return 1;
  
  //compare vector size
  //the one with a smaller vector size is smaller if positive, and the opposite for negative
  if (bits.size() < rhs.bits.size()) return negative ? 1 : -1;
  if (bits.size() > rhs.bits.size()) return negative ? -1 : 1;

  // compare each 64 bits from most significant to least significant
  for (auto i = bits.size(); i > 0; --i) {
    if (bits[i-1] < rhs.bits[i-1]) return negative ? 1 : -1;
    if (bits[i-1] > rhs.bits[i-1]) return negative ? -1 : 1;
  }
  
  return 0;
}

//convert to base_16
std::string BigInt::to_hex() const {
  if (bits.empty()){
    return "0";
  }

  std::stringstream ss;

  //iterate reversely since bits are stored from least-significant to most-significant
  for (auto it = bits.rbegin(); it != bits.rend(); ++it) {
    ss << std::hex << std::setfill('0') << std::setw(16) << *it;
  }

  std::string str = ss.str();
  int index = (int) str.length() - 1;
  for (int i = 0; i < (int) str.length() - 1; i++) {
    if (str[i] != '0') {
      index = i;
      break;
    }
  }

  str.erase(0, index);

  //Prepend negative
  if (negative) {
    str.insert(0, "-");
  }
  
  return str;
}

std::pair<BigInt, uint64_t> BigInt::divmod(uint64_t divisor) const {

  //vector storing quotient
  std::vector<uint64_t> quotient(bits.size());
  //initialize the remainder
  uint64_t remainder = 0;

  for (int i = bits.size() - 1; i >= 0; --i) {

    //use uint128_t to avoid overflow

    __uint128_t current = ((__uint128_t) remainder << 64) | bits[i];
    quotient[i] = current / divisor;
    remainder = current % divisor;

  }
  while (!quotient.empty() && quotient.back() == 0) {
    quotient.pop_back();
  }

  return {BigInt(quotient, negative), remainder};
}

std::string BigInt::to_dec() const {

  //return 0 if empty
  if (bits.empty()) return "0"; 

  BigInt number = *this;
  std::stringstream ss;

  //set the base to 10^19
  const uint64_t base = 10000000000000000000U;
  std::vector<uint64_t> digits;

  //load the remainders into digit
  while (!number.bits.empty()) {
    auto [quotient, remainder] = number.divmod(base);
    digits.push_back(remainder);
    number = quotient;
  }

  //output the digit
  if (digits.empty()) {
    ss << "0";
  } else {
    if (negative) {
      ss << "-";
    }

    //load the most significant digits first
    ss << digits.back();

    //fill the gap for the rest of them
    for (int i = digits.size() - 2; i >= 0; --i) {
      ss << std::setw(19) << std::setfill('0') << digits[i];
    }
  }

  return ss.str();
  
}
