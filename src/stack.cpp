#include "stack.hpp"

void Stack::shift_up() {
  if (base == 2) {
    registers.shift_up();
  } else {
    bit_registers.shift_up();
  }

  inEntry = false;
}

void Stack::rotate_up() {
  if (base == 2) {
    registers.rotate_up();
  } else {
    bit_registers.rotate_up();
  }
}

void Stack::rotate_down() {
  if (base == 2) {
    registers.rotate_down();
  } else {
    bit_registers.rotate_down();
  }
}

void Stack::enter() {
  afterEnter = true;
  shift_up();
}

void Stack::shift_down() {
  if (base == 2) {
    registers.shift_down();
  } else {
    bit_registers.shift_down();
  }

  inEntry = false;
}

void Stack::enter_x(uint8_t n) {
  if (base == 0 && n > 1) {
    return;
  }
  if (base == 1 && n > 7) {
    return;
  }
  if (base == 2 && n > 9) {
    return;
  }

  if (inEntry) {
    if (base == 2) {
      if (point != 0 && ipos - point >= precision)
        return;

      if (ipos - point < 10) {
        input[ipos++] = n;
      }

      convert_input();
    } else {
      uint64_t reg = bit_registers.get_x();
      if (base == 0) {
        reg <<= 1;
      } else if (base == 1) {
        reg <<= 3;
      } else if (base == 3) {
        reg <<= 4;
      }

      reg |= n;
      reg &= ~(UINT64_MAX << precision);
      bit_registers.set_x(reg);
    }

  } else {
    reset_x(n);

    if (!afterEnter) {
      shift_up();
      inEntry = true;
    } else {
      afterEnter = false;
    }

    if (base == 2) {
      convert_input();
    }
  }
}

void Stack::convert_input() {
  long double n = 0.0;
  uint64_t shift = 1;

  for (uint8_t i = 0; i < ipos; i++) {
    n += input[ipos - i - 1] * shift;
    shift *= 10;
  }

  int64_t exp = 0;
  if (point)
    exp -= (ipos - point);
  n *= powl(10.0L, exp);

  if (sign)
    n = -n;

  registers.set_x(n);
}

uint8_t Stack::get_base() { return base; }

void Stack::set_base(uint8_t b) {
  if (base == 2 && b != 2) {
    bit_registers.set_z(floorl(fabsl(registers.get_z())));
    bit_registers.set_y(floorl(fabsl(registers.get_y())));
    bit_registers.set_t(floorl(fabsl(registers.get_t())));
    convert_input();
    bit_registers.set_x(floorl(fabsl(registers.get_x())));
  } else if (b == 2 && base != 2) {
    registers.set_z(bit_registers.get_z());
    registers.set_y(bit_registers.get_y());
    registers.set_t(bit_registers.get_t());
    registers.set_x(bit_registers.get_x());

    uint64_t n = floorl(fabsl(registers.get_x()));
    int64_t exp = (uint64_t)floor(log10(n));

    ipos = 0;
    point = 0;
    sign = 0;
    input[0] = 0;

    while (exp >= 0) {
      uint8_t num = ((int)(floor(n / pow(10, exp)))) % 10;
      if (ipos < 10) {
        input[ipos++] = num;
      }
      // length += snprintf(buf + length, maxlen - length, "%d", num);
      exp--;
    }
  }

  switch (b) {
  case 1:
    precision = 12;
    break;
  case 3:
  case 0:
    precision = 16;
    break;
  case 2:
    precision = 4;
    break;
  }

  inEntry = true;
  base = b;
}

void Stack::clear_x() {
  reset_x(0);
  if (base == 2) {
    convert_input();
  }
}

void Stack::reset_x(uint8_t x) {
  if (base == 2) {
    ipos = x != 0 ? 1 : 0;
    point = 0;
    sign = 0;
    input[0] = x;
    inEntry = true;
  } else {
    bit_registers.set_x(x);
    inEntry = true;
  }
}

void Stack::enter_point() {
  if (base != 2)
    return;
  if (inEntry) {
    if (ipos == 0)
      input[ipos++] = 0;
    point = ipos;
  } else {
    reset_x(0);
  }

  if (base == 2) {
    convert_input();
  }
}

void Stack::sign_x() {
  if (!inEntry) {
    reset_x(0);
  }

  if (base == 2) {
    // for (int i = 0; i < precision; i++) {
    //   input[i] = ~input[i] & 0b1111;
    // }
    //
    // uint8_t i = precision - 1;
    // uint8_t carry = 1;
    // do {
    //   input[i] += carry;
    //   carry = (input[i] & 0b10000) >> 4;
    //   input[i] &= 0b1111;
    //   i--;
    // } while (carry == 1 && i > 0);
    sign = 1 - sign;
  } else {
    uint64_t reg = bit_registers.get_x();
    reg = ~reg;
    reg += 1;
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
  }

  if (base == 2) {
    convert_input();
  }
}

void Stack::add() {
  if (base == 2) {
    registers.set_x(registers.get_x() + registers.get_y());
  } else {
    uint64_t reg = bit_registers.get_x();
    reg += bit_registers.get_y();
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
  }
  shift_down();
}

void Stack::sub() {
  if (base == 2) {
    registers.set_x(registers.get_y() - registers.get_x());
  } else {
    uint64_t reg = bit_registers.get_x();
    reg = bit_registers.get_y() - reg;
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
  }
  shift_down();
}

void Stack::mul() {
  if (base == 2) {
    registers.set_x(registers.get_x() * registers.get_y());
  } else {
    uint64_t reg = bit_registers.get_x();
    reg *= bit_registers.get_y();
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
  }
  shift_down();
}

void Stack::div() {
  if (base == 2) {
    registers.set_x(registers.get_y() / registers.get_x());
  } else {
    uint64_t reg = bit_registers.get_x();
    reg = bit_registers.get_y() / reg;
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
  }
  shift_down();
}

void Stack::exp() {
  if (base != 2) {
    return;
  }
  registers.set_x(powl(registers.get_y(), registers.get_x()));
  shift_down();
}

void Stack::square() {
  if (base != 2) {
    return;
  }

  registers.set_x(powl(registers.get_x(), 2));
  inEntry = false;
}

void Stack::bit_shift_left() {
  if (base != 2) {
    bit_registers.set_x(bit_registers.get_x() << 1);
  }
}

void Stack::bit_shift_right() {
  if (base != 2) {
    bit_registers.set_x(bit_registers.get_x() >> 1);
  }
}

void Stack::bit_not() {
  if (base != 2) {
    uint64_t reg = bit_registers.get_x();
    reg = ~reg;
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
  }
}

void Stack::bit_and() {
  if (base != 2) {
    uint64_t reg = bit_registers.get_x();
    reg &= bit_registers.get_y();
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
    shift_down();
  }
}
void Stack::bit_or() {
  if (base != 2) {
    uint64_t reg = bit_registers.get_x();
    reg |= bit_registers.get_y();
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
    shift_down();
  }
}
void Stack::bit_xor() {
  if (base != 2) {
    uint64_t reg = bit_registers.get_x();
    reg ^= bit_registers.get_y();
    reg &= ~(UINT64_MAX << precision);
    bit_registers.set_x(reg);
    shift_down();
  }
}

void Stack::change_precision() {}

void Stack::stack_format_float(char *buf, uint8_t maxlen, uint8_t i) {
  uint8_t length = 0;
  long double n = fabsl(registers[i]) - floorl(fabsl(registers[i]));

  if (n > 0) {
    length += snprintf(buf + length, maxlen - length, ".");

    uint64_t t = (int64_t)roundl(n * powl(10.0L, precision));
    while (t % 10 == 0) {
      t /= 10;
    }
    length += snprintf(buf + length, maxlen - length, "%llu", t);
  } else if (i == 0 && point != 0) {
    length += snprintf(buf + length, maxlen - length, ".");
  }
}

void Stack::stack_format_fix(char *buf, uint8_t maxlen, uint8_t i) {
  uint8_t length = 0;
  length += snprintf(buf + length, maxlen - length, ".");

  // get all decimal places and print them out without too much math to avoid
  // floating point errors.
  long double n = fabsl(registers[i]) - floorl(fabsl(registers[i]));
  length += snprintf(buf + length, maxlen - length, "%0*llu", precision,
                     (uint64_t)roundl(n * powl(10.0L, precision)));

  // for (int x = 1; x < 13; x++) {
  //   length += snprintf(buf + length, maxlen - length, "%d",
  //                      ((int)floorl(n * powl(10.0L, x)) % 10));
  // }
}

void Stack::stack_format_dec(char *buf, uint8_t maxlen, uint8_t i) {
  // snprintf(buf, maxlen, "%c: % Lg", register_labels[i], registers[i]);
  int length = 0;
  length += snprintf(buf, maxlen, "%c:", register_labels[i]);

  if (!finitel(registers[i])) {
    snprintf(buf + length, maxlen - length, " inf");
    return;
  }

  if (isnanf(registers[i])) {
    snprintf(buf + length, maxlen - length, " nan");
    return;
  }

  if (registers[i] < 0 || (i == 0 && sign == 1)) {
    length += snprintf(buf + length, maxlen - length, "-");
  } else {
    length += snprintf(buf + length, maxlen - length, " ");
  }

  if (floorl(fabsl(registers[i])) == 0) {
    length += snprintf(buf + length, maxlen - length, "0");
  } else {
    uint64_t n = floorl(fabsl(registers[i]));
    // int64_t exp = (uint64_t)floor(log10(n));
    //
    // while (exp >= 0) {
    //   uint8_t num = ((int)(floor(n / pow(10, exp)))) % 10;
    //   length += snprintf(buf + length, maxlen - length, "%d", num);
    //   exp--;
    // }
    length += snprintf(buf + length, maxlen - length, "%llu", n);
  }

  if (display_mode == 0) {
    stack_format_float(buf + length, maxlen - length, i);
  } else {
    stack_format_fix(buf + length, maxlen - length, i);
  }
}

void Stack::stack_format_bin(char *buf, uint8_t maxlen, uint8_t i) {
  int length = 0;
  length +=
      snprintf(buf + length, maxlen - length, "%c: 0b", register_labels[i]);
  for (int x = precision - 1; x >= 0; x--) {
    uint8_t n = (bit_registers[i] >> x) & 1;
    length += snprintf(buf + length, maxlen - length, "%u", n);
  }
}

void Stack::stack_format_oct(char *buf, uint8_t maxlen, uint8_t i) {
  int length = 0;
  length += snprintf(buf, maxlen, "%c: 0o%0*llo", register_labels[i],
                     precision / 3, bit_registers[i]);
}

void Stack::stack_format_hex(char *buf, uint8_t maxlen, uint8_t i) {
  int length = 0;
  length += snprintf(buf, maxlen, "%c: 0x%0*llx", register_labels[i],
                     precision / 4, bit_registers[i]);
}

void Stack::stack_format(char *buf, uint8_t maxlen, uint8_t i) {
  switch (base) {
  case 0:
    stack_format_bin(buf, maxlen, i);
    break;
  case 1:
    stack_format_oct(buf, maxlen, i);
    break;
  case 2:
    stack_format_dec(buf, maxlen, i);
    break;
  case 3:
    stack_format_hex(buf, maxlen, i);
    break;
  }
}
