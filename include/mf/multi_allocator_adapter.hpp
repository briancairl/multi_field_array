/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

// MF
#include <mf/support/pointer_element_type.hpp>
#include <mf/support/tuple_for_each.hpp>

namespace mf
{

/**
 * @brief Handles the allocation of multiple fields
 */
template <typename ValueTs, typename AllocatorTs> class BasicMultiAllocatorAdapter;

/**
 * @copydoc BasicMultiAllocatorAdapter
 *
 *          Allocates each field with a separate allocator
 */
template <typename... ValueTs, typename... AllocatorTs>
class BasicMultiAllocatorAdapter<std::tuple<ValueTs...>, std::tuple<AllocatorTs...>>
{
  static_assert(sizeof...(ValueTs) == sizeof...(AllocatorTs), "Number of value types must match number of allocators");

public:
  using allocator_types = std::tuple<AllocatorTs...>;

  BasicMultiAllocatorAdapter() = default;
  BasicMultiAllocatorAdapter(BasicMultiAllocatorAdapter&&) = default;
  BasicMultiAllocatorAdapter(const BasicMultiAllocatorAdapter&) = default;

  /**
   * @brief Allocator state initialization constructor
   */
  explicit BasicMultiAllocatorAdapter(const std::tuple<AllocatorTs...>& initial_state) : allocators_{initial_state} {}

  /**
   * @brief Allocates memory for \c n elements of each type in \c ValueTs
   *
   *        Allocation is performed per-field type
   *
   * @param n  number of elements to allocate
   *
   * @return tuple of pointers to allocated memory each type in \c ValueTs
   */
  std::tuple<ValueTs*...> allocate(const std::size_t n)
  {
    std::tuple<ValueTs*...> ptrs;
    tuple_for_each(allocators_, ptrs, [n](auto& allocator, auto& ptr) { ptr = allocator.allocate(n); });
    return ptrs;
  }

  /**
   * @brief De-allocates memory for \c n elements of each type in \c ValueTs
   *
   *        De-allocation is performed per-field type
   *
   * @param ptr  points to memory segments to be de-allocated
   * @param n  number of elements to de-allocate
   */
  void deallocate(const std::tuple<ValueTs*...>& ptrs, const std::size_t n)
  {
    tuple_for_each(allocators_, ptrs, [n](auto& allocator, auto& ptr) { allocator.deallocate(ptr, n); });
  }

private:
  allocator_types allocators_;
};

/**
 * @brief Tag type used to specify single-allocation strategy, and which allocator type, \c AllocatorT to use
 */
template <typename AllocatorT> struct SinglePassAllocationStrategy
{};

/**
 * @copydoc BasicMultiAllocatorAdapter
 *
 *          Allocates memory for all fields at once by requesting a single memory segment
 *          which can contain all prospective elements. This memory segment will be as large
 *          as the sum of the sizes of all element value types (accounting for alignment requirements)
 *          times the number of elements requested, \c n.
 */
template <typename... ValueTs, typename ByteAllocatorT>
class BasicMultiAllocatorAdapter<std::tuple<ValueTs...>, SinglePassAllocationStrategy<ByteAllocatorT>>
{
  static_assert(
    sizeof(typename std::allocator_traits<ByteAllocatorT>::value_type) == sizeof(std::uint8_t),
    "Allocator must allocate values with same length as sizeof(std::uint8_t) (1 byte)");

public:
  using allocator_types = std::tuple<ByteAllocatorT>;

  BasicMultiAllocatorAdapter() = default;
  BasicMultiAllocatorAdapter(BasicMultiAllocatorAdapter&&) = default;
  BasicMultiAllocatorAdapter(const BasicMultiAllocatorAdapter&) = default;

  /**
   * @brief Allocator state initialization constructor
   */
  explicit BasicMultiAllocatorAdapter(const ByteAllocatorT& initial_state) : byte_allocator_{initial_state} {}

  /**
   * @brief Allocates memory for \c n elements of each type in \c ValueTs
   *
   *        Allocation is performed *ONCE* using a single, byte-allocator
   *
   * @param n  number of elements to allocate
   *
   * @return tuple of pointers to allocated memory each type in \c ValueTs
   */
  std::tuple<ValueTs*...> allocate(const std::size_t n)
  {
    std::tuple<ValueTs*...> ptrs;

    auto* byte_addr = byte_allocator_.allocate(total_allocation_length(n));

    std::size_t offset = 0UL;

    tuple_for_each(ptrs, [byte_addr, n, &offset](auto& ptr) {
      using element_type = pointer_element_t<decltype(ptr)>;
      ptr = reinterpret_cast<element_type*>(byte_addr + offset);
      offset += (sizeof(element_type) * n);
    });

    return ptrs;
  }

  /**
   * @brief De-allocates memory for \c n elements of each type in \c ValueTs
   *
   *        De-allocation is performed *ONCE* using a single, byte-allocator
   *
   * @param ptr  points to memory segments to be de-allocated
   * @param n  number of elements to de-allocate
   */
  void deallocate(const std::tuple<ValueTs*...>& ptrs, const std::size_t n)
  {
    auto* const byte_addr = reinterpret_cast<std::uint8_t*>(std::get<0>(ptrs));
    byte_allocator_.deallocate(byte_addr, total_allocation_length(n));
  }

private:
  /**
   * @brief Returns the required length of an allocated buffer which can \c n values of each type
   *
   *        Length should account for alignment
   */
  static constexpr std::size_t total_allocation_length(const std::size_t n)
  {
    return sizeof(std::tuple<ValueTs...>) * n;
  }

  ByteAllocatorT byte_allocator_;
};

/**
 * @brief Convenience type alias which creates an BasicMultiAllocatorAdapter with \c std::allocator for each provided
 * type
 */
template <typename... ValueTs>
using multi_allocator_adapter =
  BasicMultiAllocatorAdapter<std::tuple<ValueTs...>, std::tuple<std::allocator<ValueTs>...>>;

/**
 * @brief Convenience type alias which creates an BasicMultiAllocatorAdapter for single-pass allocation with \c
 * std::allocator<std::uint8_t>
 */
template <typename... ValueTs>
using single_allocator_adapter =
  BasicMultiAllocatorAdapter<std::tuple<ValueTs...>, SinglePassAllocationStrategy<std::allocator<std::uint8_t>>>;

}  // namespace mf
