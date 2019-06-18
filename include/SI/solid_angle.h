#pragma once

#include "detail/number_parser.h"
#include "detail/unit.h"

/// todo add rad and sr as literal operators

namespace SI {

template <typename _Type, typename _Ratio>
using solid_angle_t = detail::unit_t<'R', 1, _Type, _Ratio>;

template <typename _Type>
using atto_sterradiant_t = solid_angle_t<_Type, std::atto>;
template <typename _Type>
using femto_sterradiant_t = solid_angle_t<_Type, std::femto>;
template <typename _Type>
using pico_sterradiant_t = solid_angle_t<_Type, std::pico>;
template <typename _Type>
using nano_sterradiant_t = solid_angle_t<_Type, std::nano>;
template <typename _Type>
using micro_sterradiant_t = solid_angle_t<_Type, std::micro>;
template <typename _Type>
using milli_sterradiant_t = solid_angle_t<_Type, std::milli>;
template <typename _Type>
using sterradiant_t = solid_angle_t<_Type, std::ratio<1>>;

inline namespace literals {

template <char... _Digits> constexpr auto operator""_asr() {
  return atto_sterradiant_t<int64_t>{
      SI::detail::parsing::Number<_Digits...>::value};
}

template <char... _Digits> constexpr auto operator""_fsr() {
  return femto_sterradiant_t<int64_t>{
      SI::detail::parsing::Number<_Digits...>::value};
}

template <char... _Digits> constexpr auto operator""_psr() {
  return pico_sterradiant_t<int64_t>{
      SI::detail::parsing::Number<_Digits...>::value};
}

template <char... _Digits> constexpr auto operator""_nsr() {
  return nano_sterradiant_t<int64_t>{
      SI::detail::parsing::Number<_Digits...>::value};
}

template <char... _Digits> constexpr auto operator""_usr() {
  return micro_sterradiant_t<int64_t>{
      SI::detail::parsing::Number<_Digits...>::value};
}

template <char... _Digits> constexpr auto operator""_msr() {
  return milli_sterradiant_t<int64_t>{
      SI::detail::parsing::Number<_Digits...>::value};
}

template <char... _Digits> constexpr auto operator""_sr() {
  return sterradiant_t<int64_t>{SI::detail::parsing::Number<_Digits...>::value};
}

constexpr auto operator""_asr(long double value) {
  return atto_sterradiant_t<long double>{value};
}

constexpr auto operator""_fsr(long double value) {
  return femto_sterradiant_t<long double>{value};
}

constexpr auto operator""_psr(long double value) {
  return pico_sterradiant_t<long double>{value};
}

constexpr auto operator""_nsr(long double value) {
  return nano_sterradiant_t<long double>{value};
}

constexpr auto operator""_usr(long double value) {
  return micro_sterradiant_t<long double>{value};
}

constexpr auto operator""_msr(long double value) {
  return milli_sterradiant_t<long double>{value};
}

constexpr auto operator""_sr(long double value) {
  return sterradiant_t<long double>{value};
}

} // namespace literals

} // namespace SI