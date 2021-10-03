/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <iterator>
#include <tuple>
#include <type_traits>

// MF
#include <mf/support/tuple_wrap.hpp>

namespace mf
{
namespace detail
{

/**
 * @brief Returns a tuple of lvalue references from a tuple of pointers, which a dereferenced
 */
template <typename TupleOfIteratorsT, std::size_t... Indices>
inline auto tuple_dereference(TupleOfIteratorsT&& tup_of_ptr, std::index_sequence<Indices...> _)
{
  // Generate a tuple of reference types from original tuple of pointer types

  // clang-format off
  using TupleOfReferencesT =
    std::tuple<
      typename std::iterator_traits<
        std::tuple_element_t<
          Indices,
          std::remove_reference_t<TupleOfIteratorsT>
        >
      >::reference...
    >;
  // clang-format on

  // Pack references into tuple from dereferenced pointers
  return TupleOfReferencesT{(*std::get<Indices>(std::forward<TupleOfIteratorsT>(tup_of_ptr)))...};
}

}  // namespace detail


/**
 * @copydoc detail::tuple_dereference
 */
template <typename TupleOfIteratorsT> inline auto tuple_dereference(TupleOfIteratorsT&& tup_of_ptr)
{
  static constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<TupleOfIteratorsT>>;
  return detail::tuple_dereference(std::forward<TupleOfIteratorsT>(tup_of_ptr), std::make_index_sequence<N>{});
}

template <typename TupleT> using tuple_of_pointers = tuple_wrap<TupleT, std::add_pointer>;

template <typename TupleT> using tuple_of_pointers_t = typename tuple_of_pointers<TupleT>::type;

template <typename TupleT> using tuple_of_lvalue_references = tuple_wrap<TupleT, std::add_lvalue_reference>;

template <typename TupleT> using tuple_of_lvalue_references_t = typename tuple_of_lvalue_references<TupleT>::type;

}  // namespace mf
