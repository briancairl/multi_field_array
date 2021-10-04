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
 * @brief Invokes a callback, \c cb, for elements of each tuple, \c tups
 *
 *        Callback should take N args, where N is \c sizeof...(tups)
 *        @code{.cpp}
 *        tuple_for_each(
 *          [](const auto& tuple1_element, const auto& tuple2_element, ..., const auto& tupleN_element)
 *          {
 *          },
 *          tuple1,
 *          tuple2,
 *          ...,
 *          tupleN
 *        );
 *        @endcode
 */
template <typename CallbackT, typename... TupleTs> void tuple_for_each(CallbackT&& cb, TupleTs&&... tups);

namespace detail
{

/**
 * @brief Generates (and statically checks) common size between tuples, \c TupleTs
 */
template <typename... TupleTs> struct CommonTupleSize;

/**
 * @copydoc CommonTupleSize
 *
 *          Termination case
 */
template <typename FirstTupleT> struct CommonTupleSize<FirstTupleT>
{
  static constexpr std::size_t size = std::tuple_size_v<std::remove_reference_t<FirstTupleT>>;
};

/**
 * @copydoc CommonTupleSize
 *
 *          Size-checking case
 */
template <typename FirstTupleT, typename SecondTupleT, typename... OtherTupleTs>
struct CommonTupleSize<FirstTupleT, SecondTupleT, OtherTupleTs...>
{
  static constexpr std::size_t size = std::tuple_size_v<std::remove_reference_t<FirstTupleT>>;
  static_assert(size == CommonTupleSize<SecondTupleT, OtherTupleTs...>::size, "tuple sizes do not match");
};

/**
 * @brief Calls \c std::apply on a tuple formed of elements at each \c TupleTs at \c Index
 */
template <std::size_t Index> struct MultiTupleApplyAdapter
{
  template <typename CallbackT, typename... TupleTs> inline static void exec(CallbackT&& cb, TupleTs&&... tups)
  {
    std::apply(std::forward<CallbackT>(cb), std::forward_as_tuple(std::get<Index>(std::forward<TupleTs>(tups))...));
  }
};

/**
 * @copydoc tuple_for_each
 * @warn implementation
 */
template <typename CallbackT, std::size_t... Indices, typename... TupleTs>
inline int tuple_for_each(CallbackT&& cb, std::index_sequence<Indices...> _, TupleTs&&... tups)
{
  return (
    (MultiTupleApplyAdapter<Indices>::template exec(std::forward<CallbackT>(cb), std::forward<TupleTs>(tups)...), 1) +
    ...);
}

}  // namespace detail

/**
 * @copydoc tuple_for_each
 * @warn implementation
 */
template <typename CallbackT, typename... TupleTs> inline void tuple_for_each(CallbackT&& cb, TupleTs&&... tups)
{
  static const std::size_t IterationCount = detail::CommonTupleSize<TupleTs...>::size;

  [[maybe_unused]] const auto __iteration_count = detail::tuple_for_each(
    std::forward<CallbackT>(cb), std::make_index_sequence<IterationCount>{}, std::forward<TupleTs>(tups)...);
}

}  // namespace mf
