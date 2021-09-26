/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

// MF
#include <mf/support/tuple_allocator_rebind.hpp>
#include <mf/support/tuple_for_each.hpp>

namespace mf
{

template <typename... Ts> using Fields = std::tuple<Ts...>;

template <typename FieldTs, typename IndexT, typename AllocatorT = std::allocator<IndexT>> class MultiFieldArray;

template <typename... Ts, typename IndexT, typename AllocatorT> class MultiFieldArray<Fields<Ts...>, IndexT, AllocatorT>
{
public:
  MultiFieldArray() : allocators_{}, size_{0}, capacity_{0}
  {
    tuple_for_each(data_, [](auto& ptr) { ptr = nullptr; });
  }

  explicit MultiFieldArray(std::size_t count) : allocators_{}, size_{count}, capacity_{count}
  {
    tuple_for_each(allocators_, data_, [count](auto& allocator, auto& ptr) {
      ptr = allocator.allocate(count);

      using PointerType = std::remove_reference_t<decltype(ptr)>;
      using ElementType = typename std::pointer_traits<PointerType>::element_type;

      if constexpr (!std::is_fundamental<ElementType>())
      {
        std::for_each(ptr, ptr + count, [](auto& element) { new (std::addressof(element)) ElementType{}; });
      }
    });
  }

  ~MultiFieldArray()
  {
    tuple_for_each(allocators_, data_, [s = size_, c = capacity_](auto& allocator, auto& ptr) {
      using PointerType = std::remove_reference_t<decltype(ptr)>;
      using ElementType = typename std::pointer_traits<PointerType>::element_type;

      if constexpr (!std::is_fundamental<ElementType>())
      {
        std::for_each(ptr, ptr + s, [](auto& element) { element.~ElementType(); });
      }
      allocator.deallocate(ptr, c);
    });
  }

  template <typename ValueT> inline ValueT& at(const IndexT index) { return std::get<ValueT*>(data_)[index]; }

  template <typename ValueT> inline const ValueT& at(const IndexT index) const
  {
    return std::get<ValueT*>(data_)[index];
  }

  template <typename... ValueTs> inline std::tuple<ValueTs&...> get(const IndexT index)
  {
    return std::tuple<ValueTs&...>{std::get<ValueTs*>(data_)[index]...};
  }

  template <typename... ValueTs> inline std::tuple<const ValueTs&...> get(const IndexT index) const
  {
    return std::tuple<const ValueTs&...>{std::get<ValueTs*>(data_)[index]...};
  }

  /**
   * @brief Creates a new element at the end of the array(s)
   */
  template <typename... FieldCTorTupleTs>
  inline void emplace_back(std::piecewise_construct_t _, FieldCTorTupleTs&&... ctor_args)
  {
    MultiFieldArray::check_realloc();

    static_assert(sizeof...(Ts) == sizeof...(FieldCTorTupleTs), "Should be construct args for each type");

    tuple_for_each(
      data_,
      std::forward_as_tuple(std::forward<FieldCTorTupleTs>(ctor_args)...),
      [s = size_](auto& ptr, auto&& ctor_args) {
        using PointerType = std::remove_reference_t<decltype(ptr)>;
        using ElementType = typename std::pointer_traits<PointerType>::element_type;

        if constexpr (std::is_fundamental_v<ElementType>)
        {
          *(ptr + s) = std::get<0>(ctor_args);
        }
        else
        {}
        // new (ptr + s) ElementType{};
      });

    ++size_;
  }

  /**
   * @brief Creates a new element at the end of the array(s)
   */
  inline IndexT create_back()
  {
    MultiFieldArray::check_realloc();

    tuple_for_each(data_, [s = size_](auto& ptr) {
      using PointerType = std::remove_reference_t<decltype(ptr)>;
      using ElementType = typename std::pointer_traits<PointerType>::element_type;

      new (ptr + s) ElementType{};
    });

    return size_++;
  }

  /**
   * @brief Returns true when element count is zero (container is empty)
   */
  inline bool empty() const { return size_ == 0; }

  /**
   * @brief Returns the number of elements
   */
  inline std::size_t size() const { return size_; }

  /**
   * @brief Returns the number of elements that can be held in currently allocated storage
   */
  inline std::size_t capacity() const { return capacity_; }

  /**
   * @brief Direct access to the underlying array
   */
  template <std::size_t Index> inline const auto* data() const { return std::get<Index>(data_); }

  /**
   * @brief Direct access to the underlying array
   */
  template <typename ValueT> inline const ValueT* data() const { return std::get<ValueT*>(data_); }

private:
  /**
   * @brief Checks if addding next element exceed capacity; reallocated if it does
   */
  inline void check_realloc()
  {
    if (capacity_ < size_ + 1)
    {
      const std::size_t prev_capacity = capacity_;

      // TODO(enhancement) allow for custom reallocation policy
      capacity_ *= 2;
      capacity_ += 2;

      // Cache the previous pointer address
      std::tuple<Ts*...> data_tmp{data_};

      // Allocate new memory with increase capacity
      tuple_for_each(allocators_, data_, [c = capacity_](auto& allocator, auto& ptr) { ptr = allocator.allocate(c); });

      // Copy old data into newly allocated buffer
      tuple_for_each(data_, data_tmp, [s = size_](auto& new_ptr, const auto& original_ptr) {
        auto* l_ptr = new_ptr;
        const auto* r_ptr = original_ptr;
        const auto* const r_ptr_last = original_ptr + s;
        while (r_ptr != r_ptr_last)
        {
          *l_ptr = *r_ptr;
          ++l_ptr;
          ++r_ptr;
        }
      });

      // Delete old buffers if previously allocated
      if (!MultiFieldArray::empty())
      {
        tuple_for_each(allocators_, data_tmp, [s = size_, prev_capacity](auto& allocator, auto& ptr) {
          using PointerType = std::remove_reference_t<decltype(ptr)>;
          using ElementType = typename std::pointer_traits<PointerType>::element_type;

          if constexpr (!std::is_fundamental<ElementType>())
          {
            std::for_each(ptr, ptr + s, [](auto& element) { element.~ElementType(); });
          }

          allocator.deallocate(ptr, prev_capacity);
        });
      }
    }
  }

  /// Allocator
  tuple_allocator_rebind_t<AllocatorT, Ts...> allocators_;

  /// Pointers to data for each field
  std::tuple<Ts*...> data_;

  /// The effective number of elements in each component array
  std::size_t size_;

  /// The current capacity of each component array
  std::size_t capacity_;
};

}  // namespace mf
