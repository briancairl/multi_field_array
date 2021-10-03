/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <algorithm>
#include <cstring>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

// MF
#include <mf/multi_field_array_fwd.hpp>
#include <mf/multi_field_array_view.hpp>
#include <mf/support/pointer_element_type.hpp>
#include <mf/support/tuple_for_each.hpp>
#include <mf/support/tuple_select.hpp>

namespace mf
{

template <typename... Ts, typename... AllocatorTs>
class BasicMultiFieldArray<std::tuple<Ts...>, std::tuple<AllocatorTs...>>
{
public:
  /// Allocators for all \c Ts
  using allocator_types = std::tuple<AllocatorTs...>;

  /// Tuple of field value types
  using value_types = std::tuple<Ts...>;

  /**
   * @brief Default constructor
   *
   *        Sets initialize size and capacity to zero
   */
  BasicMultiFieldArray() : allocators_{}, size_{0}, capacity_{0}
  {
    tuple_for_each(data_, [](auto& ptr) { ptr = nullptr; });
  }

  explicit BasicMultiFieldArray(const allocator_types& allocators) : allocators_{allocators}, size_{0}, capacity_{0}
  {
    tuple_for_each(data_, [](auto& ptr) { ptr = nullptr; });
  }

  explicit BasicMultiFieldArray(std::size_t count) : allocators_{}, size_{count}, capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count);
  }

  BasicMultiFieldArray(std::size_t count, const allocator_types& allocators) :
      allocators_{allocators},
      size_{count},
      capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count);
  }

  template <typename CTorArgTupleT>
  BasicMultiFieldArray(std::size_t count, CTorArgTupleT&& ctor_arg_tuple) :
      allocators_{},
      size_{count},
      capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count, std::forward<CTorArgTupleT>(ctor_arg_tuple));
  }

  template <typename CTorArgTupleT>
  BasicMultiFieldArray(std::size_t count, const allocator_types& allocators, CTorArgTupleT&& ctor_arg_tuple) :
      allocators_{allocators},
      size_{count},
      capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count, std::forward<CTorArgTupleT>(ctor_arg_tuple));
  }

  BasicMultiFieldArray(const BasicMultiFieldArray& other) :
      allocators_{other.allocators_},
      size_{other.size_},
      capacity_{other.capacity_}
  {
    if (other.empty())
    {
      // If "other" is empty, initialize buffer pointers to null to prevent deallocation
      tuple_for_each(data_, [](auto& ptr) { ptr = nullptr; });
    }
    else
    {
      // Allocate new buffers with the same capacity as "other" array
      BasicMultiFieldArray::allocate(data_, capacity_);

      // Copy-construct new elements from values in buffers of "other"
      tuple_for_each(data_, other.data_, [s = size_](auto* dst_ptr, const auto* src_ptr) {
        using ElementType = pointer_element_t<decltype(dst_ptr)>;

        // Call default constructor for non-fundamental types
        if constexpr (std::is_fundamental_v<ElementType>)
        {
          std::memcpy(dst_ptr, src_ptr, sizeof(ElementType) * s);
        }
        else
        {
          auto* const dst_last_ptr = dst_ptr + s;
          for (; dst_ptr != dst_last_ptr; ++dst_ptr, ++src_ptr)
          {
            new (dst_ptr) ElementType{*src_ptr};
          }
        }
      });
    }
  }

  BasicMultiFieldArray(BasicMultiFieldArray&& other) :
      allocators_{std::move(other.allocators_)},
      size_{other.size_},
      capacity_{other.capacity_}
  {
    // "Steal" data pointer from "other"
    tuple_for_each(data_, other.data_, [](auto& dst_ptr, auto& src_ptr) {
      dst_ptr = src_ptr;
      src_ptr = nullptr;
    });

    // Set "other" to a fully-reset state
    other.size_ = 0UL;
    other.capacity_ = 0UL;
  }

  ~BasicMultiFieldArray()
  {
    // Don't deallocate if capacity is zero
    if (capacity_ == 0UL)
    {
      return;
    }

    // Destroy all elements which have been constructed and deallocate buffer memory
    tuple_for_each(allocators_, data_, [s = size_, c = capacity_](auto& allocator, auto& ptr) {
      using ElementType = pointer_element_t<decltype(ptr)>;

      if constexpr (!std::is_fundamental<ElementType>())
      {
        std::for_each(ptr, ptr + s, [](auto& element) { element.~ElementType(); });
      }
      allocator.deallocate(ptr, c);
    });
  }

  /**
   * @brief Returns references to values at index for each specified field type
   *
   * @returns A tuple of references to fields if multiple types are specified, otherwise,
   *          returns a single reference
   */
  template <typename... ValueTs> inline decltype(auto) get(const std::size_t index)
  {
    static_assert(sizeof...(ValueTs) != 0, "ValueTs must not be empty");

    if constexpr (sizeof...(ValueTs) == 1)
    {
      using SingleValueT = std::tuple_element_t<0, std::tuple<ValueTs...>>;
      return static_cast<SingleValueT&>(std::get<SingleValueT*>(data_)[index]);
    }
    else
    {
      return std::tuple<ValueTs&...>{std::get<ValueTs*>(data_)[index]...};
    }
  }

  /**
   * @copydoc get
   * @note const qualified version
   */
  template <typename... ValueTs> inline decltype(auto) get(const std::size_t index) const
  {
    static_assert(sizeof...(ValueTs) != 0, "ValueTs must not be empty");

    if constexpr (sizeof...(ValueTs) == 1)
    {
      using SingleValueT = std::tuple_element_t<0, std::tuple<ValueTs...>>;
      return static_cast<const SingleValueT&>(std::get<SingleValueT*>(data_)[index]);
    }
    else
    {
      return std::tuple<ValueTs&...>{std::get<ValueTs*>(data_)[index]...};
    }
  }

  /**
   * @brief Creates a new element at the end of the array(s)
   *
   *        This version participates in overload resolution if \c std::piecewise_construct
   *        is the first argument. All arguments which follow should be tuples are arguments
   *        to be forwarded to the construct for each corresponding field type.
   * \n
   *        @code{.cpp}
   *        BasicMultiFieldArray<std::tuple<float, int, std::string>> array;
   *
   *        array.emplace_back(
   *          std::piecewise_construct,         // tag
   *          std::forward_as_tuple(0.f),       // float field
   *          std::forward_as_tuple(1),         // int field
   *          std::forward_as_tuple("a string") // std::string field
   *        );
   *        @endcode
   */
  template <typename... PiecewiseFieldCTorTupleTs>
  inline void emplace_back(std::piecewise_construct_t _, PiecewiseFieldCTorTupleTs&&... ctor_args)
  {
    static_assert(sizeof...(Ts) == sizeof...(PiecewiseFieldCTorTupleTs), "Should be construct args for each type");

    BasicMultiFieldArray::check_and_realloc_for_element_added();

    // Contruct new element past the previous last element in allocated buffers
    tuple_for_each(data_, std::forward_as_tuple(ctor_args...), [s = size_](auto& ptr, auto&& ctor_arg_tuple) {
      using ElementType = pointer_element_t<decltype(ptr)>;

      // Simply assign fundamental types
      if constexpr (std::is_fundamental_v<ElementType>)
      {
        static_assert(
          std::tuple_size_v<std::remove_reference_t<decltype(ctor_arg_tuple)>> == 1UL,
          "Cannot assign fundamental type from multiple values");

        *(ptr + s) = std::get<0>(std::forward<decltype(ctor_arg_tuple)>(ctor_arg_tuple));
      }
      // Call constructor for non-fundamental types
      else
      {
        // TODO(fixup) Does the result in the same code as constructing in-place?
        new (ptr + s)
          ElementType{std::make_from_tuple<ElementType>(std::forward<decltype(ctor_arg_tuple)>(ctor_arg_tuple))};
      }
    });

    // Increment the known size of the buffers in terms of effective elements
    ++size_;
  }

  /**
   * @brief Creates a new element at the end of the array(s)
   *
   *        This version participates in overload resolution if \c std::piecewise_construct
   *        is NOT the first argument. All arguments which follow should be lvalue or rvalue
   *        references to values used to copy/move construct values for each corresponding
   *        field type
   * \n
   *        @code{.cpp}
   *        BasicMultiFieldArray<std::tuple<float, int, std::string>> array;
   *
   *        array.emplace_back(
   *          0.f,       // float field
   *          1,         // int field
   *          std::string{"a string"} // std::string field
   *        );
   *        @endcode
   */
  template <typename... FieldCopyOrMoveCTorTupleTs>
  inline void emplace_back(FieldCopyOrMoveCTorTupleTs&&... copy_or_move_ctor_args)
  {
    static_assert(
      (sizeof...(FieldCopyOrMoveCTorTupleTs) == sizeof...(Ts)) or (sizeof...(FieldCopyOrMoveCTorTupleTs) == 0UL),
      "Number of argments must be 0 or match the number of field types");

    BasicMultiFieldArray::check_and_realloc_for_element_added();

    // Contruct new element past the previous last element in allocated buffers
    if constexpr (sizeof...(FieldCopyOrMoveCTorTupleTs) == sizeof...(Ts))
    {
      // Call copy/move constructors for any non-trivial types
      tuple_for_each(data_, std::forward_as_tuple(copy_or_move_ctor_args...), [s = size_](auto& ptr, auto&& ctor_arg) {
        using ElementType = pointer_element_t<decltype(ptr)>;

        // Call default constructor for non-fundamental types
        if constexpr (std::is_fundamental_v<ElementType>)
        {
          *(ptr + s) = std::forward<decltype(ctor_arg)>(ctor_arg);
        }
        else
        {
          new (ptr + s) ElementType{std::forward<decltype(ctor_arg)>(ctor_arg)};
        }
      });
    }
    else
    {
      tuple_for_each(data_, [s = size_](auto& ptr) {
        using ElementType = pointer_element_t<decltype(ptr)>;

        // Call default constructor for non-fundamental types
        if constexpr (!std::is_fundamental_v<ElementType>)
        {
          new (ptr + s) ElementType{};
        }
      });
    }

    // Increment the known size of the buffers in terms of effective elements
    ++size_;
  }

  /**
   * @brief Resizes each field array to the given size
   *
   *        Non-fundemental field types are default-constructed
   */
  template <typename... CTorArgTupleT> void resize(const std::size_t new_size, CTorArgTupleT&&... ctor_arg_tuple)
  {
    static_assert(sizeof...(CTorArgTupleT) < 2, "ctor_arg_tuple must be a tuple");

    // Do nothing if requested size is the same as our previous size
    if (new_size == size_)
    {
      return;
    }
    // Increase capacity if new size is larger than previous capacity
    else if (new_size >= capacity_)
    {
      // Pointers to new data
      std::tuple<Ts*...> new_data;

      // Allocate new memory and default construct
      BasicMultiFieldArray::allocate_and_construct(new_data, new_size, std::forward<CTorArgTupleT>(ctor_arg_tuple)...);

      // Destroy old elements
      BasicMultiFieldArray::destroy(data_, size_);

      // Deallocate old buffers
      BasicMultiFieldArray::deallocate(data_, capacity_);

      // Reassign buffers
      data_ = new_data;
      capacity_ = new_size;
    }
    // Destroy trailing elements if new size is smaller than previous size
    else if (new_size < size_)
    {
      tuple_for_each(data_, [s = new_size, e = size_](auto& ptr) {
        using ElementType = pointer_element_t<decltype(ptr)>;

        // Call destructor for non-fundamental types
        if constexpr (!std::is_fundamental_v<ElementType>)
        {
          std::for_each(ptr + s, ptr + e, [](auto& element) { element.~ElementType(); });
        }
      });
    }
    // Default-construct trailing elements if new size is larger than previous size
    else  // (new_size > size_)
    {
      tuple_for_each(data_, [s = new_size, e = size_](auto& ptr) {
        using ElementType = pointer_element_t<decltype(ptr)>;

        // Call default constructor for non-fundamental types
        if constexpr (!std::is_fundamental_v<ElementType>)
        {
          std::for_each(ptr + s, ptr + e, [](auto& element) { new (std::addressof(element)) ElementType{}; });
        }
      });
    }

    // Set new size
    size_ = new_size;
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

  /**
   * @brief Returns first iterator which iterates over all field simulatenously
   */
  inline auto begin() { return view().begin(); }

  /**
   * @brief Returns one-past last iterator which iterates over all field simulatenously
   */
  inline auto end() { return view().end(); }

  /**
   * @brief Returns first iterator which iterates over all field simulatenously
   */
  inline auto begin() const { return view().begin(); }

  /**
   * @brief Returns one-past last iterator which iterates over all field simulatenously
   */
  inline auto end() const { return view().end(); }

  /**
   * @brief Returns iterator (pointer) to first element in a particular field sub-array
   */
  template <std::size_t Index> inline auto* begin() { return std::get<Index>(data_); }

  /**
   * @brief Returns iterator (pointer) to first element in a particular field sub-array
   */
  template <typename ValueT> inline ValueT* begin() { return std::get<ValueT*>(data_); }

  /**
   * @brief Returns iterator (pointer) to first element in a particular field sub-array
   */
  template <std::size_t Index> inline const auto* begin() const { return std::get<Index>(data_); }

  /**
   * @brief Returns iterator (pointer) to first element in a particular field sub-array
   */
  template <typename ValueT> inline const ValueT* begin() const { return std::get<ValueT*>(data_); }

  /**
   * @brief Returns iterator (pointer) to one past last element in a particular field sub-array
   */
  template <std::size_t Index> inline auto* end() { return BasicMultiFieldArray::template begin<Index>() + size_; }

  /**
   * @brief Returns iterator (pointer) to one past last element in a particular field sub-array
   */
  template <typename ValueT> inline ValueT* end() { return BasicMultiFieldArray::template begin<ValueT>() + size_; }

  /**
   * @brief Returns iterator (pointer) to one past last element in a particular field sub-array
   */
  template <std::size_t Index> inline const auto* end() const
  {
    return BasicMultiFieldArray::template begin<Index>() + size_;
  }

  /**
   * @brief Returns iterator (pointer) to one past last element in a particular field sub-array
   */
  template <typename ValueT> inline const ValueT* end() const
  {
    return BasicMultiFieldArray::template begin<ValueT>() + size_;
  }

  /**
   * @brief Returns allocator for given field array
   */
  template <std::size_t Index> constexpr auto get_allocator() const { return std::get<Index>(allocators_); }

  /**
   * @copydoc get_allocator
   */
  template <typename ValueT> constexpr auto get_allocator() const { return std::get<ValueT>(allocators_); }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  MultiFieldArrayView<std::tuple<Ts...>> view() { return MultiFieldArrayView<std::tuple<Ts...>>{data_, size_}; }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  template <typename... ViewValueTs> MultiFieldArrayView<std::tuple<ViewValueTs...>> view()
  {
    return MultiFieldArrayView<std::tuple<ViewValueTs...>>{std::forward_as_tuple(std::get<ViewValueTs*>(data_)...),
                                                           size_};
  }

  /**
   * @copydoc view
   */
  template <std::size_t... Indices> MultiFieldArrayView<tuple_select_t<value_types, Indices...>> view()
  {
    return MultiFieldArrayView<tuple_select_t<value_types, Indices...>>{
      std::forward_as_tuple(std::get<Indices>(data_)...), size_};
  }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  MultiFieldArrayView<std::tuple<const Ts...>> view() const
  {
    return MultiFieldArrayView<std::tuple<const Ts...>>{data_, size_};
  }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  template <typename... ViewValueTs> MultiFieldArrayView<std::tuple<const ViewValueTs...>> view() const
  {
    return MultiFieldArrayView<std::tuple<const ViewValueTs...>>{
      std::forward_as_tuple(const_cast<const ViewValueTs*>(std::get<ViewValueTs*>(data_))...), size_};
  }

  /**
   * @copydoc view
   */
  template <std::size_t... Indices> MultiFieldArrayView<const_tuple_select_t<value_types, Indices...>> view() const
  {
    return MultiFieldArrayView<const_tuple_select_t<value_types, Indices...>>{
      std::forward_as_tuple(std::get<Indices>(data_)...), size_};
  }

private:
  /**
   * @brief Allocates memory to \c buffers
   */
  inline void allocate(std::tuple<Ts*...>& buffers, const std::size_t capacity)
  {
    tuple_for_each(
      allocators_, buffers, [capacity](auto& allocator, auto& ptr) { ptr = allocator.allocate(capacity); });
  };

  /**
   * @brief Allocates memory to \c buffers and constructs those values
   */
  inline void allocate_and_construct(std::tuple<Ts*...>& buffers, const std::size_t capacity)
  {
    // Allocate new memory with specified capacity
    BasicMultiFieldArray::allocate(buffers, capacity);

    // Default construct new elements
    tuple_for_each(buffers, [capacity](auto& ptr) {
      using ElementType = pointer_element_t<decltype(ptr)>;

      // Call default constructor for non-fundamental types
      if constexpr (!std::is_fundamental_v<ElementType>)
      {
        std::for_each(ptr, ptr + capacity, [](auto& element) { new (std::addressof(element)) ElementType{}; });
      }
    });
  };

  /**
   * @brief Allocates memory to \c buffers and constructs those values
   */
  template <typename CTorArgTupleT>
  inline void
  allocate_and_construct(std::tuple<Ts*...>& buffers, const std::size_t capacity, CTorArgTupleT&& ctor_arg_tuple)
  {
    // Allocate new memory with specified capacity
    BasicMultiFieldArray::allocate(buffers, capacity);

    // Value construct new elements
    tuple_for_each(buffers, std::forward<CTorArgTupleT>(ctor_arg_tuple), [capacity](auto& ptr, const auto& other) {
      using ElementType = pointer_element_t<decltype(ptr)>;

      // Call default constructor for non-fundamental types
      if constexpr (std::is_fundamental_v<ElementType>)
      {
        std::fill(ptr, ptr + capacity, other);
      }
      else
      {
        std::for_each(
          ptr, ptr + capacity, [&other](auto& element) { new (std::addressof(element)) ElementType{other}; });
      }
    });
  };

  /**
   * @brief Deallocates memory from \c buffers
   */
  inline void deallocate(std::tuple<Ts*...>& buffers, const std::size_t length)
  {
    tuple_for_each(allocators_, buffers, [length](auto& allocator, auto& ptr) {
      allocator.deallocate(ptr, length);
      ptr = nullptr;
    });
  };

  /**
   * @brief Moves-constructor elements to \c buffers
   */
  inline void move_construct(std::tuple<Ts*...>& buffers, const std::size_t length)
  {
    tuple_for_each(buffers, data_, [length](auto* dst_ptr, auto* src_ptr) {
      using ElementType = pointer_element_t<decltype(dst_ptr)>;

      if constexpr (std::is_fundamental_v<ElementType>)
      {
        std::memcpy(dst_ptr, src_ptr, sizeof(ElementType) * length);
      }
      else
      {
        const auto* const last_src_ptr = src_ptr + length;
        while (src_ptr != last_src_ptr)
        {
          new (dst_ptr) ElementType{std::move(*src_ptr)};
          ++src_ptr;
          ++dst_ptr;
        }
      }
    });
  }

  /**
   * @brief Calls destructor on elements from \c buffers
   */
  inline void destroy(std::tuple<Ts*...>& buffers, const std::size_t length)
  {
    tuple_for_each(buffers, [length](auto& ptr) {
      using ElementType = pointer_element_t<decltype(ptr)>;

      if constexpr (!std::is_fundamental<ElementType>())
      {
        std::for_each(ptr, ptr + length, [](auto& element) { element.~ElementType(); });
      }
    });
  }

  /**
   * @brief Checks if addding next element exceed capacity; reallocated if it does
   */
  inline void check_and_realloc_for_element_added()
  {
    // Check if we need to reallocate data storage buffers
    if (capacity_ >= size_ + 1)
    {
      return;
    }

    // TODO(enhancement) allow for custom reallocation policy
    const std::size_t new_capacity = (2UL * capacity_) + 2UL;

    // Pointers to new data
    std::tuple<Ts*...> new_data;

    // Allocate new memory with increase capacity
    BasicMultiFieldArray::allocate(new_data, new_capacity);

    // Move old elements to new buffer
    BasicMultiFieldArray::move_construct(new_data, size_);

    // Destroy old elements
    BasicMultiFieldArray::destroy(data_, size_);

    // Deallocate old buffers
    BasicMultiFieldArray::deallocate(data_, capacity_);

    // Reassign buffers
    data_ = new_data;
    capacity_ = new_capacity;
  }

  /// Allocators for each field array
  allocator_types allocators_;

  /// Pointers to data for each field
  std::tuple<Ts*...> data_;

  /// The effective number of elements in each component array
  std::size_t size_;

  /// The current capacity of each component array
  std::size_t capacity_;
};

template <typename... FieldTs>
using multi_field_array = BasicMultiFieldArray<std::tuple<FieldTs...>, std::tuple<std::allocator<FieldTs>...>>;

}  // namespace mf
