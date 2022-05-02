/**
 * @copyright 2022 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <tuple>
#include <type_traits>

namespace mf
{
namespace detail
{

template <typename ClassT, typename PtrT, typename ArgsTupleT, std::size_t... Indices>
void apply_placement_new(PtrT* ptr, ArgsTupleT&& arg_tup, std::integer_sequence<std::size_t, Indices...>)
{
  using ArgsTupleValueT = std::remove_reference_t<std::remove_const_t<ArgsTupleT>>;
  new (ptr) ClassT{std::forward<std::tuple_element_t<Indices, ArgsTupleValueT>>(
    std::get<Indices>(std::forward<ArgsTupleT>(arg_tup)))...};
}

}  // namespace detail

template <typename ClassT, typename PtrT, typename ArgsTupleT>
void apply_placement_new(PtrT&& ptr, ArgsTupleT&& arg_tup)
{
  static constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<std::remove_const_t<ArgsTupleT>>>;
  detail::apply_placement_new<ClassT>(
    std::forward<PtrT>(ptr), std::forward<ArgsTupleT>(arg_tup), std::make_integer_sequence<std::size_t, N>());
}

}  // namespace mf
