#pragma once

#include <limits>

namespace SI::detail::parsing {

/// @todo add assert for numeric overflow

/// struct converting a char digit into an int
template <intmax_t _base, char _Str_digit> struct Digit_impl {
  static_assert((_Str_digit >= '0' && _Str_digit <= '9') ||
                (_Str_digit >= 'a' && _Str_digit <= 'f') ||
                (_Str_digit >= 'A' && _Str_digit <= 'F') || _Str_digit == '\'');

  static_assert(_base >= 2);

  static constexpr bool is_valid_digit = _Str_digit == '\'' ? false : true;
  static constexpr intmax_t value =
      (_Str_digit >= '0' && _Str_digit <= '9')
          ? _Str_digit - '0'
          : (_Str_digit >= 'a' && _Str_digit <= 'f')
                ? 10 + (_Str_digit - 'a')
                : (_Str_digit >= 'A' && _Str_digit <= 'F')
                      ? 10 + (_Str_digit - 'A')
                      : std::numeric_limits<intmax_t>::max();
  static_assert(!is_valid_digit || value < _base, "Digit is valid for base");
};

template <intmax_t _base, char _Str_digit>
struct Digit : public Digit_impl<_base, _Str_digit> {};

/*template <intmax_t _base> struct Digit<_base, '\''> {
  using is_valid_digit = std::false_type;
  static constexpr intmax_t value = 0;
};*/

template <intmax_t _base, char _digit, char... _digits> struct Power_impl {

  using digit = Digit<_base, _digit>;
  using recursive_power = Power_impl<_base, _digits...>;
  static constexpr intmax_t power = digit::is_valid_digit
                                        ? (recursive_power::power * _base)
                                        : recursive_power::power;
};

// terminating case for power variadic template
template <intmax_t _base, char _digit> struct Power_impl<_base, _digit> {
  static constexpr intmax_t power = 1;
};

// interface class for power calculation
template <intmax_t _base, char... _digits>
struct Power : Power_impl<_base, _digits...> {};

template <intmax_t _base> struct Power<_base> {
  static constexpr intmax_t power = 1;
};

// recursive struct that builds the number
template <intmax_t _base, char _digit, char... _digits> struct Number_impl {

  static constexpr intmax_t magnitude = sizeof...(_digits);
  static constexpr intmax_t base = _base;
  using digit = Digit<base, _digit>;
  static constexpr intmax_t power = Power<base, _digit, _digits...>::power;

  using recursive_number = Number_impl<_base, _digits...>;
  static constexpr intmax_t value =
      (digit::is_valid_digit ? (digit::value * power) : 0) +
      recursive_number::value;
};

// terminating case for variadic template
template <intmax_t _base, char _digit> struct Number_impl<_base, _digit> {

  using digit = Digit<_base, _digit>;
  static_assert(digit::is_valid_digit);
  static constexpr intmax_t value = digit::value;
  static constexpr intmax_t magnitude = 0;
  static constexpr intmax_t base = _base;
  static constexpr intmax_t power = Power<base>::power;
  static_assert(power == 1, "power should be one");
};

/// interface class for number
template <char... _digits> struct Number : Number_impl<10, _digits...> {};

/// specialisation of Number for hex notation
template <char... _digits>
struct Number<'0', 'x', _digits...> : Number_impl<16, _digits...> {};

/// specialisation of Number for hex notation
template <char... _digits>
struct Number<'0', 'X', _digits...> : Number_impl<16, _digits...> {};

/// specialisation of Number for binary notation
template <char... _digits>
struct Number<'0', 'b', _digits...> : Number_impl<2, _digits...> {};

/// specialisation of Number for binary notation
template <char... _digits>
struct Number<'0', 'B', _digits...> : Number_impl<2, _digits...> {};

/// specialisation of Number for octal notation
template <char... _digits>
struct Number<'0', _digits...> : Number_impl<8, _digits...> {};

} // namespace SI::detail::parsing