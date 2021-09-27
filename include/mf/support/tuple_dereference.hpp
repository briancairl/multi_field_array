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

/**
 * @brief Returns a tuple of lvalue references from a tuple of pointers, which a dereferenced
 */
template <typename TupleOfPointersT, std::size_t... Indices>
inline auto tuple_dereference(TupleOfPointersT&& tup_of_ptr, std::index_sequence<Indices...> _)
{
  // Generate a tuple of reference types from original tuple of pointer types

  // clang-format off
  using ReferenceTupleT =
    std::tuple<
      std::add_lvalue_reference_t<
        std::remove_pointer_t<
          std::tuple_element_t<
            Indices,
            std::remove_reference_t<TupleOfPointersT>
          >
        >
      >...
    >;
  // clang-format on

  // Pack references into tuple from dereferenced pointers
  return ReferenceTupleT{*std::get<Indices>(std::forward<TupleOfPointersT>(tup_of_ptr))...};
}

}  // namespace detail


/**
 * @copydoc detail::tuple_dereference
 */
template <typename TupleOfPointersT> inline auto tuple_dereference(TupleOfPointersT&& tup_of_ptr)
{
  static constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<TupleOfPointersT>>;
  return detail::tuple_dereference(std::forward<TupleOfPointersT>(tup_of_ptr), std::make_index_sequence<N>{});
}

}  // namespace mf
