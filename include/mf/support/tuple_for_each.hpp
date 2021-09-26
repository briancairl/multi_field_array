/**
 * @copyright 2021 MF
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

template <typename TupleT, typename UnaryCallbackT, std::size_t... Indices>
inline int tuple_for_each(TupleT&& tup, UnaryCallbackT&& unary_cb, std::index_sequence<Indices...> _)
{
  return ((unary_cb(std::get<Indices>(tup)), 1) + ...);
}

template <typename Tuple1T, typename Tuple2T, typename BinaryCallbackT, std::size_t... Indices>
inline int
tuple_for_each(Tuple1T&& tup1, Tuple2T&& tup2, BinaryCallbackT&& binary_cb, std::index_sequence<Indices...> _)
{
  return ((binary_cb(std::get<Indices>(tup1), std::get<Indices>(tup2)), 1) + ...);
}

}  // namespace detail

template <typename TupleT, typename UnaryCallbackT> inline void tuple_for_each(TupleT&& tup, UnaryCallbackT&& unary_cb)
{
  static constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<TupleT>>;

  detail::tuple_for_each(
    std::forward<TupleT>(tup), std::forward<UnaryCallbackT>(unary_cb), std::make_index_sequence<N>{});
}

template <typename Tuple1T, typename Tuple2T, typename BinaryCallbackT>
inline void tuple_for_each(Tuple1T&& tup1, Tuple2T&& tup2, BinaryCallbackT&& binary_cb)
{
  static constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple1T>>;

  static_assert(N == std::tuple_size_v<std::remove_reference_t<Tuple2T>>, "Tuple1T and Tuple2T must be the same size");

  detail::tuple_for_each(
    std::forward<Tuple1T>(tup1),
    std::forward<Tuple2T>(tup2),
    std::forward<BinaryCallbackT>(binary_cb),
    std::make_index_sequence<N>{});
}

}  // namespace mf
