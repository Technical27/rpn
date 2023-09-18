#include <cstdint>
#include <type_traits>

template <class T> class Register {
  static_assert(std::is_arithmetic<T>::value, "Register type must be number");

public:
  Register() = default;
  Register(Register &&) = default;
  Register(const Register &) = default;
  Register &operator=(Register &&) = default;
  Register &operator=(const Register &) = default;
  ~Register() = default;

  // T operator[](uint8_t i);
  //
  // T get_x();
  // T get_y();
  // T get_z();
  // T get_t();
  //
  // void set_x(T n);
  // void set_y(T n);
  // void set_z(T n);
  // void set_t(T n);
  //
  // void shift_up();
  // void shift_down();

  T get_x() { return regs[0]; }

  T get_y() { return regs[1]; }

  T get_z() { return regs[2]; }

  T get_t() { return regs[3]; }

  void set_x(T n) { regs[0] = n; }

  void set_y(T n) { regs[1] = n; }

  void set_z(T n) { regs[2] = n; }

  void set_t(T n) { regs[3] = n; }

  void shift_up() {
    regs[3] = regs[2];
    regs[2] = regs[1];
    regs[1] = regs[0];
  }

  void shift_down() {
    regs[1] = regs[2];
    regs[2] = regs[3];
  }

  void rotate_up() {
    T n = regs[3];
    regs[3] = regs[2];
    regs[2] = regs[1];
    regs[1] = regs[0];
    regs[0] = n;
  }

  void rotate_down() {
    T n = regs[0];
    regs[0] = regs[1];
    regs[1] = regs[2];
    regs[2] = regs[3];
    regs[3] = n;
  }

  T operator[](uint8_t i) { return regs[i]; }

private:
  T regs[4] = {0, 0, 0, 0};
};
