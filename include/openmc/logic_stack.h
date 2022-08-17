#ifndef OPENMC_LOGIC_STACK
#define OPENMC_LOGIC_STACK

#include <cstdint>

namespace openmc {

class LogicStack {
public:
  //@{
  //! Typedefs
  using value_type = bool;
  using size_type = std::size_t;
  //@}

  //! Default constructor
  LogicStack() {}

  //! Greatest number of boolean values allowed on the stack
  static constexpr size_type max_stack_depth() { return sizeof(size_type) * 8; }

  //// ACCESSORS ////

  //! Number of elements on the stack
  size_type size() const { return size_; }

  // Whether any elements exist
  bool empty() const { return size_ == size_type(0); }

  // Access the top value of the stack
  auto top() const -> value_type { return lsb(data_); }

  // Access a single bit (zero is deepest level of stack), used by ostream
  auto operator[](size_type index) const -> value_type
  {
    size_type shift = size() - index - size_type(1);
    return LogicStack::lsb(data_ >> shift);
  }

  //// MUTATORS ////

  // Push a boolean onto the stack
  void push(value_type v)
  {
    // Shift stack left and add least significant bit
    data_ = LogicStack::shl(data_) | LogicStack::lsb(v);
    // Size for DBC
    ++size_;
  }

  // Pop a value off the stack
  auto pop() -> value_type
  {
    // Extract least significant bit
    value_type result = LogicStack::lsb(data_);
    // Shift right
    data_ = LogicStack::shr(data_);
    // Update size
    --size_;
    return result;
  }

  // Negate the value on the top of the stack
  void apply_not() { data_ ^= size_type(1); }

  // Apply boolean 'and' to the top of the stack
  void apply_and()
  {
    size_type temp = LogicStack::lsb(data_);
    data_ = LogicStack::shr(data_) & (temp | ~size_type(1));
    --size_;
  }

  // Apply boolean 'or' to the top of the stack
  void apply_or()
  {
    data_ = LogicStack::shr(data_) | LogicStack::lsb(data_);
    --size_;
  }

private:
  //! Get the least significant bit
  static constexpr size_type lsb(size_type val) { return val & size_type(1); }

  //! Shift right by one
  static constexpr size_type shr(size_type val) { return val >> size_type(1); }

  //! Shift left by one
  static constexpr size_type shl(size_type val) { return val << size_type(1); }

  //// Data ////

  size_type data_ {0}; //!< Stack data
  size_type size_ {0}; //!< Stack depth
};

} // namespace openmc

#endif // OPENMC_LOGIC_STACK
