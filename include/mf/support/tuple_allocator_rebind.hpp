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

template <typename AllocatorT, typename... Ts> struct tuple_allocator_rebind
{
  using type = std::tuple<typename std::allocator_traits<AllocatorT>::template rebind_alloc<Ts>...>;
};

template <typename AllocatorT, typename... Ts>
using tuple_allocator_rebind_t = typename tuple_allocator_rebind<AllocatorT, Ts...>::type;

}  // namespace mf
