#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "register.hpp"

class Stack {
public:
  Stack() = default;
  Stack(Stack &&) = default;
  Stack(const Stack &) = default;
  Stack &operator=(Stack &&) = default;
  Stack &operator=(const Stack &) = default;
  ~Stack() = default;

  void enter_x(uint8_t);
  void clear_x();
  void enter_point();

  void shift_up();
  void shift_down();

  void rotate_up();
  void rotate_down();

  void enter();

  void add();
  void sub();
  void mul();
  void div();

  void exp();
  void square();

  void bit_shift_left();
  void bit_shift_right();

  void bit_not();
  void bit_and();
  void bit_or();
  void bit_xor();

  void sign_x();

  void change_precision();

  void set_base(uint8_t b);

  void stack_format(char *buf, uint8_t maxlen, uint8_t i);

  uint8_t get_base();

  uint8_t get_mode();

private:
  Register<long double> registers;
  Register<uint64_t> bit_registers;
  char register_labels[4] = {'X', 'Y', 'Z', 'T'};

  uint8_t base = 2;
  uint8_t display_mode = 0;

  uint8_t precision = 4;

  bool inEntry = true;
  bool afterEnter = false;
  uint8_t input[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t ipos = 0;
  uint8_t point = 0;
  uint8_t sign = 0;

  void convert_input();

  void reset_x(uint8_t x);

  void stack_format_dec(char *buf, uint8_t maxlen, uint8_t i);
  void stack_format_float(char *buf, uint8_t maxlen, uint8_t i);
  void stack_format_fix(char *buf, uint8_t maxlen, uint8_t i);

  void stack_format_hex(char *buf, uint8_t maxlen, uint8_t i);

  void stack_format_bin(char *buf, uint8_t maxlen, uint8_t i);

  void stack_format_oct(char *buf, uint8_t maxlen, uint8_t i);
};
