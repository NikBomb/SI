#pragma once

#include "detail.h"
#include <ratio>
#include <type_traits>

namespace SI {

// forward declarations
template <typename _T, typename _rhs_T>
constexpr auto unit_cast(const _rhs_T &rhs);

template <typename _unit_lhs, typename _unit_rhs> struct unit_with_common_ratio;

/**
 * @brief base template class for holding values of type _Type to be multiplied
 *with a ratio _Ratio
 *
 * @tparam _Symbol dimension symbol of the unit (see
 *https://en.wikipedia.org/wiki/SI_base_unit)
 * @tparam _Ratio Ratio in relation to the base unit of a SI unit
 * @tparam _Type internal representation of the value
 * @tparam _Exponent the exponent to the unit (i.e. length ==  m^1, area == m^2,
 *volume = m^3)
 **/
template <
    char _Symbol, char _Exponent = 1, class _Ratio = std::ratio<1>,
    typename _Type = int64_t,
    typename std::enable_if<_Ratio::num == 1 || _Ratio::den == 1>::type * =
        nullptr,
    typename std::enable_if<std::is_arithmetic<_Type>::value>::type * = nullptr>

struct unit_t {
  static_assert(_Exponent != 0, "Exponent is non-zero");
  static_assert(detail::is_ratio<_Ratio>::value, "_Ratio is a std::ratio");
  using ratio = _Ratio;
  using internal_type = _Type;
  using exponent = std::integral_constant<char, _Exponent>;
  using symbol = std::integral_constant<char, _Symbol>;

  /// Construct with value v
  constexpr unit_t(_Type v) : value_{v} {}

  /// returns the stored value as raw type
  constexpr internal_type raw_value() const { return value_; }

  /// Comparison operator takes considers different ratios, i.e. 1000 micros ===
  /// 1 milli
  template <class _rhs_Ratio = std::ratio<1>>
  constexpr bool
  operator==(const unit_t<symbol::value, exponent::value, _rhs_Ratio,
                          internal_type> &rhs) const {

    static_assert(std::is_integral<internal_type>::value ||
                      std::is_floating_point<internal_type>::value,
                  "Is integral or floating point");

    if constexpr (std::is_integral<internal_type>::value) {
      return unit_cast<unit_t<_Symbol, _Exponent, _Ratio, _Type>>(rhs)
                 .raw_value() == value_;
    } else {
      return std::abs(unit_cast<unit_t<_Symbol, _Exponent, _Ratio, _Type>>(rhs)
                          .raw_value() -
                      value_) < std::numeric_limits<internal_type>::epsilon();
    }
  }

  template <class _rhs_Ratio = std::ratio<1>>
  constexpr bool
  operator!=(const unit_t<symbol::value, exponent::value, _rhs_Ratio,
                          internal_type> &rhs) const {
    return !(*this == rhs);
  }

  template <class _rhs_Ratio>
  constexpr bool operator<(const unit_t<symbol::value, exponent::value,
                                        _rhs_Ratio, internal_type> &rhs) const {
    return unit_cast<unit_t<_Symbol, _Exponent, _Ratio, _Type>>(rhs)
               .raw_value() < raw_value();
  }

  template <class _rhs_Ratio>
  constexpr bool operator>(const unit_t<symbol::value, exponent::value,
                                        _rhs_Ratio, internal_type> &rhs) const {
    return unit_cast<unit_t<_Symbol, _Exponent, _Ratio, _Type>>(rhs)
               .raw_value() > raw_value();
  }

  /// multiply with a non-unit scalar
  constexpr unit_t operator*(const _Type f) const { return {value_ * f}; }

  template <char _rhs_Exponent>
  constexpr auto operator*(const unit_t<symbol::value, _rhs_Exponent, ratio,
                                        internal_type> &rhs) const {
    return unit_t<symbol::value, _rhs_Exponent + exponent::value, ratio,
                  internal_type>{raw_value() * rhs.raw_value()};
  }

  /// multiply with a same unit, with possibly different exponent and different
  /// ratio resulting unit is the same as 'this'/left hand side of operation
  template <char _rhs_Exponent, typename _rhs_Ratio>
  constexpr auto operator*(const unit_t<symbol::value, _rhs_Exponent,
                                        _rhs_Ratio, internal_type> &rhs) const {
    using rhs_t = typename std::remove_reference<decltype(rhs)>::type;

    return unit_t<symbol::value, exponent::value + rhs_t::exponent::value,
                  ratio, internal_type>{
        value_ *
        unit_cast<unit_t<_Symbol, _Exponent, _Ratio, _Type>>(rhs).raw_value()};
  }

  /// multiply with a same unit
  /// resulting unit is the same as 'this'/left hand side of operation
  template <char _rhs_symbol, char _rhs_exponent, typename _rhs_Ratio,
            typename _rhs_type,
            typename std::enable_if<_rhs_exponent != exponent::value>::type * =
                nullptr,
            typename std::enable_if<_rhs_symbol == _Symbol>::type * = nullptr,
            typename std::enable_if<std::is_same<_Type, _rhs_type>::value>::type
                * = nullptr>
  constexpr auto operator/(const unit_t<_rhs_symbol, _rhs_exponent, _rhs_Ratio,
                                        _rhs_type> &rhs) const {
    using rhs_t = typename std::remove_reference<decltype(rhs)>::type;

    return unit_t<symbol::value, exponent::value - rhs_t::exponent::value,
                  ratio, internal_type>{
        value_ /
        unit_cast<unit_t<_Symbol, _Exponent, _Ratio, _Type>>(rhs).raw_value()};
  }

  /// if the same units of the same exponent are divided then the result is a
  /// scalar
  /// @todo use unit_cast internally
  template <char _rhs_symbol, char _rhs_exponent, typename _rhs_Ratio,
            typename _rhs_type,
            typename std::enable_if<_rhs_exponent == exponent::value>::type * =
                nullptr,
            typename std::enable_if<_rhs_symbol == _Symbol>::type * = nullptr,
            typename std::enable_if<std::is_same<_Type, _rhs_type>::value>::type
                * = nullptr>
  constexpr _Type operator/(const unit_t<_rhs_symbol, _rhs_exponent, _rhs_Ratio,
                                         _rhs_type> &rhs) const {

    return raw_value() / rhs.raw_value();
  }

  template <typename _rhs_Ratio>
  constexpr unit_t
  operator+(const unit_t<symbol::value, exponent::value, _rhs_Ratio,
                         internal_type> &rhs) const {
    return unit_t{
        raw_value() +
        unit_cast<unit_t<_Symbol, _Exponent, _Ratio, _Type>>(rhs).raw_value()};
  }

  /// negate operation
  constexpr unit_t operator-() { return {-value_}; }

private:
  internal_type value_;
};

/// operator to divide
template <char _Symbol, char _Exponent, class _Ratio, typename _Type = int64_t>
constexpr auto operator/(_Type lhs,
                         const unit_t<_Symbol, _Exponent, _Ratio, _Type> &rhs) {

  return unit_cast<unit_t<_Symbol, -_Exponent, _Ratio, _Type>>(
      unit_t<_Symbol, -_Exponent, std::ratio<1>, _Type>{
          lhs / (rhs.raw_value() * (_Ratio::num / _Ratio::den))});
}

/// helper template to check if a type is a unit_t (false for all other types)
template <typename _Tp> struct is_unit_t : std::false_type {};

/// template specialisation to check if a type is a unit_t (true if unit_t)
template <char _Symbol, char _Exponent, class _Ratio, typename _Type>
struct is_unit_t<const unit_t<_Symbol, _Exponent, _Ratio, _Type>>
    : std::true_type {};

/// non-const specialisation of check above
template <char _Symbol, char _Exponent, class _Ratio, typename _Type>
struct is_unit_t<unit_t<_Symbol, _Exponent, _Ratio, _Type>> : std::true_type {};

/// function to cast between two units of the same type
template <typename _T, typename _rhs_T>
constexpr auto unit_cast(const _rhs_T &rhs) {
  // using static assert instead of std::enable if in order to be able to
  // forward declare this function easier
  static_assert(
      is_unit_t<_rhs_T>::value ||
          std::is_base_of<
              unit_t<_rhs_T::symbol::value, _rhs_T::exponent::value,
                     typename _rhs_T::ratio, typename _rhs_T::internal_type>,
              _rhs_T>::value,
      "is of type unit_t or a derived class");
  using conversion_ratio =
      std::ratio_divide<typename _rhs_T::ratio, typename _T::ratio>;

  return _T{(rhs.raw_value() * conversion_ratio::num / conversion_ratio::den)};
}

/// @todo consider specializing std::common type
/// as described here
/// https://stackoverflow.com/questions/36523038/stdcommon-type-trait-for-user-defined-types
/// or as std::chrono does
template <typename _ratio_lhs, typename _ratio_rhs> struct ratio_gcd {
private:
  typedef std::__static_gcd<_ratio_lhs::num, _ratio_rhs::num> __gcd_num;
  typedef std::__static_gcd<_ratio_lhs::den, _ratio_rhs::den> __gcd_den;

public:
  typedef std::ratio<__gcd_num::value,
                     (_ratio_lhs::den / __gcd_den::value) * _ratio_rhs::den>
      ratio;
};

template <typename _unit_lhs, typename _unit_rhs>
struct unit_with_common_ratio {
  static_assert(is_unit_t<_unit_lhs>::value, "only supported for SI::unit_t");
  static_assert(is_unit_t<_unit_rhs>::value, "only supported for SI::unit_t");
  typedef unit_t<_unit_lhs::symbol::value, _unit_lhs::exponent::value,
                 typename ratio_gcd<typename _unit_lhs::ratio,
                                    typename _unit_rhs::ratio>::ratio,
                 typename _unit_lhs::internal_type>
      type;
};

} // namespace SI
