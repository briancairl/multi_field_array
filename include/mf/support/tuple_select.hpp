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

/**
 * @brief Helper used to select a subset of tuple element types by index to create a new tuple type
 */
template <typename TupleT, std::size_t... Indices> struct tuple_select
{
  using type = std::tuple<std::tuple_element_t<Indices, TupleT>...>;
};

/**
 * @brief Convenience alias for tuple_select
 */
template <typename TupleT, std::size_t... Indices>
using tuple_select_t = typename tuple_select<TupleT, Indices...>::type;

/**
 * @brief Helper used to select a subset of tuple element types by index to create a new tuple type with \c const
 * elements
 */
template <typename TupleT, std::size_t... Indices> struct const_tuple_select
{
  using type = std::tuple<const std::tuple_element_t<Indices, TupleT>...>;
};

/**
 * @brief Convenience alias for const_tuple_select
 */
template <typename TupleT, std::size_t... Indices>
using const_tuple_select_t = typename const_tuple_select<TupleT, Indices...>::type;

}  // namespace mf
