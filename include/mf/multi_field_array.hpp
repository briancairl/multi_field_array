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
#include <mf/multi_allocator_adapter.hpp>
#include <mf/multi_field_array_fwd.hpp>
#include <mf/support/pointer_element_type.hpp>
#include <mf/support/tuple_for_each.hpp>
#include <mf/support/tuple_select.hpp>
#include <mf/support/view.hpp>

namespace mf
{

template <typename... Ts, typename AllocatorTs, typename CapacityIncreasePolicy>
class BasicMultiFieldArray<
  std::tuple<Ts...>,
  BasicMultiAllocatorAdapter<std::tuple<Ts...>, AllocatorTs>,
  CapacityIncreasePolicy>
{
public:
  /// Alias for multi-field allocator adapter
  using allocator_adapter_type = BasicMultiAllocatorAdapter<std::tuple<Ts...>, AllocatorTs>;

  /// Tuple of field value types
  using value_types = std::tuple<Ts...>;

  /**
   * @brief Default constructor
   *
   *        Sets initialize size and capacity to zero
   */
  BasicMultiFieldArray() : allocator_adapter_{}, size_{0}, capacity_{0}
  {
    tuple_for_each([](auto& ptr) { ptr = nullptr; }, data_);
  }

  explicit BasicMultiFieldArray(const allocator_adapter_type& allocator_adapter) :
      allocator_adapter_{allocator_adapter},
      size_{0},
      capacity_{0}
  {
    tuple_for_each([](auto& ptr) { ptr = nullptr; }, data_);
  }

  explicit BasicMultiFieldArray(std::size_t count) : allocator_adapter_{}, size_{count}, capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count);
  }

  BasicMultiFieldArray(std::size_t count, const allocator_adapter_type& allocator_adapter) :
      allocator_adapter_{allocator_adapter},
      size_{count},
      capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count);
  }

  template <typename CTorArgTupleT>
  BasicMultiFieldArray(std::size_t count, CTorArgTupleT&& ctor_arg_tuple) :
      allocator_adapter_{},
      size_{count},
      capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count, std::forward<CTorArgTupleT>(ctor_arg_tuple));
  }

  template <typename CTorArgTupleT>
  BasicMultiFieldArray(
    std::size_t count,
    const allocator_adapter_type& allocator_adapter,
    CTorArgTupleT&& ctor_arg_tuple) :
      allocator_adapter_{allocator_adapter},
      size_{count},
      capacity_{count}
  {
    BasicMultiFieldArray::allocate_and_construct(data_, count, std::forward<CTorArgTupleT>(ctor_arg_tuple));
  }

  BasicMultiFieldArray(const BasicMultiFieldArray& other) :
      allocator_adapter_{other.allocator_adapter_},
      size_{other.size_},
      capacity_{other.capacity_}
  {
    if (other.empty())
    {
      // If "other" is empty, initialize buffer pointers to null to prevent deallocation
      tuple_for_each([](auto& ptr) { ptr = nullptr; }, data_);
    }
    else
    {
      // Allocate new buffers with the same capacity as "other" array
      BasicMultiFieldArray::allocate(data_, capacity_);

      // Copy-construct new elements from values in buffers of "other"
      tuple_for_each(
        [s = size_](auto* dst_ptr, const auto* src_ptr) {
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
        },
        data_,
        other.data_);
    }
  }

  BasicMultiFieldArray(BasicMultiFieldArray&& other) :
      allocator_adapter_{std::move(other.allocator_adapter_)},
      size_{other.size_},
      capacity_{other.capacity_}
  {
    // "Steal" data pointer from "other"
    tuple_for_each(
      [](auto& dst_ptr, auto& src_ptr) {
        dst_ptr = src_ptr;
        src_ptr = nullptr;
      },
      data_,
      other.data_);

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

    // Destroy old elements
    BasicMultiFieldArray::destroy(data_, size_);

    // Deallocate old buffers
    BasicMultiFieldArray::deallocate(data_, capacity_);
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
   *        multi_field_array<std::tuple<float, int, std::string>> array;
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
    tuple_for_each(
      [s = size_](auto& ptr, auto&& ctor_arg_tuple) {
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
      },
      data_,
      std::forward_as_tuple(ctor_args...));

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
   *        multi_field_array<std::tuple<float, int, std::string>> array;
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
      tuple_for_each(
        [s = size_](auto& ptr, auto&& ctor_arg) {
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
        },
        data_,
        std::forward_as_tuple(copy_or_move_ctor_args...));
    }
    else
    {
      tuple_for_each(
        [s = size_](auto& ptr) {
          using ElementType = pointer_element_t<decltype(ptr)>;

          // Call default constructor for non-fundamental types
          if constexpr (!std::is_fundamental_v<ElementType>)
          {
            new (ptr + s) ElementType{};
          }
        },
        data_);
    }

    // Increment the known size of the buffers in terms of effective elements
    ++size_;
  }

  /**
   * @brief Allocates memory for at least \c new_capacity elements
   *
   *        If \c new_capacity is larger than \c capacity(), then array memory is reallocated to fit new,
   *        old elements are copied and new elements are constructed. Element count, \c size(), is unchanged.
   *        If \c new_capacity is smaller than or equal to \c capacity(), then capacity is unchanged.
   */
  void reserve(const std::size_t new_capacity)
  {
    // Do nothing if requested capacity is the less than or equal to our previous capacity
    if (new_capacity <= capacity_)
    {
      return;
    }
    // Increase capacity if new capacity is larger than previous capacity
    else if (new_capacity > capacity_)
    {
      // Pointers to new data
      std::tuple<Ts*...> new_data;

      // Allocate new memory and default construct
      BasicMultiFieldArray::allocate(new_data, new_capacity);

      // Move old data to new buffers
      BasicMultiFieldArray::move_construct(new_data, size_);

      // Destroy old elements
      BasicMultiFieldArray::destroy(data_, size_);

      // Deallocate old buffers
      BasicMultiFieldArray::deallocate(data_, capacity_);

      // Reassign buffers
      data_ = new_data;
      capacity_ = new_capacity;
    }
  }

  /**
   * @brief Resizes each field array to the given size, \c new_size
   *
   *        If \c new_size is larger than \c size(), then new elements are constructed.
   *        If \c new_size is larger than \c capacity(), then array memory is reallocated to fit new,
   *        old elements are copied and new elements are constructed.
   *        If \c new_size is smaller than \c size(), then all tail elements past \c new_size
   *        are destroyed and capacity is unchanged.
   *        If \c new_size is equal to \c size, then the container is unchanged
   * \n
   *        May be called with a single argument if all fields are default constructable, like so:
   * \n
   *        @code{.cpp}
   *        multi_field_array<std::tuple<float, int, std::string>> array;
   *        array.resize(5);
   *        @endcode
   * \n
   *        May also be called with two arguments, where the second argument is a tuple of values to
   *        intialize all newly created values on resizing
   * \n
   *        @code{.cpp}
   *        multi_field_array<std::tuple<float, int, std::string>> array;
   *        array.resize(5, std::forward_as_tuple(1.f, 2, "3"));
   *        @endcode
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
      BasicMultiFieldArray::allocate(new_data, new_size);

      // Move old elements to new buffer
      BasicMultiFieldArray::move_construct(new_data, size_);

      // Construct new elements at the end of the buffer
      {
        auto start_p = new_data;
        tuple_for_each([offset = size_](auto& ptr) { ptr += offset; }, start_p);
        BasicMultiFieldArray::construct(start_p, new_size - size_, std::forward<CTorArgTupleT>(ctor_arg_tuple)...);
      }

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
      auto start_p = data_;
      tuple_for_each([offset = new_size](auto& ptr) { ptr += offset; }, start_p);
      BasicMultiFieldArray::destroy(start_p, size_ - new_size);
    }
    // Default-construct trailing elements if new size is larger than previous size
    else  // (new_size > size_)
    {
      auto start_p = data_;
      tuple_for_each([offset = size_](auto& ptr) { ptr += offset; }, start_p);
      BasicMultiFieldArray::construct(start_p, new_size - size_, std::forward<CTorArgTupleT>(ctor_arg_tuple)...);
    }

    // Set new size
    size_ = new_size;
  }

  /**
   * @brief Clears all elements, setting effective size to 0
   *
   *        Does not change current \c capacity()
   */
  inline void clear()
  {
    // Don't clear if size is zero
    if (size_ == 0UL)
    {
      return;
    }

    // Destroy old elements
    BasicMultiFieldArray::destroy(data_, size_);

    // Size effective size to zero
    size_ = 0UL;
  }

  /**
   * @brief Clears all elements, setting effective size to 0 and deallocates all memory
   *
   *        Sets \c capacity() to 0
   */
  inline void release()
  {
    // Clear all elements
    BasicMultiFieldArray::clear();

    // Deallocate old buffers
    BasicMultiFieldArray::deallocate(data_, capacity_);
    capacity_ = 0UL;
  }

  /**
   * @brief Returns true when element count is zero (container is empty)
   */
  inline bool empty() const { return size_ == 0; }

  /**
   * @brief Returns the number of elements
   *
   *        i.e. \c std::distance(begin(), end())
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
   * @brief Returns first iterator which iterates over all field simulatenously
   */
  inline auto cbegin() const { return view().begin(); }

  /**
   * @brief Returns one-past last iterator which iterates over all field simulatenously
   */
  inline auto cend() const { return view().end(); }

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
   * @brief Returns iterator (pointer) to first element in a particular field sub-array
   */
  template <std::size_t Index> inline const auto* cbegin() const { return std::get<Index>(data_); }

  /**
   * @brief Returns iterator (pointer) to first element in a particular field sub-array
   */
  template <typename ValueT> inline const ValueT* cbegin() const { return std::get<ValueT*>(data_); }

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
   * @brief Returns iterator (pointer) to one past last element in a particular field sub-array
   */
  template <std::size_t Index> inline const auto* cend() const
  {
    return BasicMultiFieldArray::template begin<Index>() + size_;
  }

  /**
   * @brief Returns iterator (pointer) to one past last element in a particular field sub-array
   */
  template <typename ValueT> inline const ValueT* cend() const
  {
    return BasicMultiFieldArray::template begin<ValueT>() + size_;
  }

  /**
   * @brief Returns allocator adapter
   */
  constexpr allocator_adapter_type get_allocator_adapter() const { return allocator_adapter_; }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  View<std::tuple<Ts...>> view() { return View<std::tuple<Ts...>>{data_, size_}; }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  template <typename... ViewValueTs> View<std::tuple<ViewValueTs...>> view()
  {
    return View<std::tuple<ViewValueTs...>>{std::forward_as_tuple(std::get<ViewValueTs*>(data_)...), size_};
  }

  /**
   * @copydoc view
   */
  template <std::size_t... Indices> View<tuple_select_t<value_types, Indices...>> view()
  {
    return View<tuple_select_t<value_types, Indices...>>{std::forward_as_tuple(std::get<Indices>(data_)...), size_};
  }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  View<std::tuple<const Ts...>> view() const { return View<std::tuple<const Ts...>>{data_, size_}; }

  /**
   * @brief Returns an iterable data view for one or more types contained within the original array
   */
  template <typename... ViewValueTs> View<std::tuple<const ViewValueTs...>> view() const
  {
    return View<std::tuple<const ViewValueTs...>>{
      std::forward_as_tuple(const_cast<const ViewValueTs*>(std::get<ViewValueTs*>(data_))...), size_};
  }

  /**
   * @copydoc view
   */
  template <std::size_t... Indices> View<const_tuple_select_t<value_types, Indices...>> view() const
  {
    return View<const_tuple_select_t<value_types, Indices...>>{std::forward_as_tuple(std::get<Indices>(data_)...),
                                                               size_};
  }

  /**
   * @brief Returns a reference to the element at specified location \c pos. No bounds checking is performed.
   *
   * @param pos  element position
   *
   * @return reference to element at \c pos
   */
  inline auto operator[](const std::size_t pos) { return view()[pos]; }

  /**
   * @copydoc operator[]
   */
  inline auto operator[](const std::size_t pos) const { return view()[pos]; }

  /**
   * @brief Returns a reference to the element at specified location \c pos. Bounds checking is performed.
   *
   * @param pos  element position
   *
   * @return reference to element at \c pos
   *
   * @throws \c std::out_of_range  if \c pos exceeds bounds of the array
   */
  inline auto at(const std::size_t pos) { return view().at(pos); }

  /**
   * @copydoc at
   */
  inline auto at(const std::size_t pos) const { return view().at(pos); }

private:
  /**
   * @brief Allocates memory to \c buffers
   */
  inline void allocate(std::tuple<Ts*...>& buffers, const std::size_t capacity)
  {
    buffers = allocator_adapter_.allocate(capacity);
  };

  /**
   * @brief Invokes default constructor on \n n element in \c buffers
   */
  inline void construct(std::tuple<Ts*...>& buffers, const std::size_t n)
  {
    // Default construct new elements
    tuple_for_each(
      [n](auto& ptr) {
        using ElementType = pointer_element_t<decltype(ptr)>;

        // Call default constructor for non-fundamental types
        if constexpr (!std::is_fundamental_v<ElementType>)
        {
          std::for_each(ptr, ptr + n, [](auto& element) { new (std::addressof(element)) ElementType{}; });
        }
      },
      buffers);
  };

  /**
   * @brief Invokes copy constructor on \n n element in \c buffers
   */
  template <typename CTorArgTupleT>
  inline void construct(std::tuple<Ts*...>& buffers, const std::size_t n, CTorArgTupleT&& ctor_arg_tuple)
  {
    // Value construct new elements
    tuple_for_each(
      [n](auto& ptr, const auto& other) {
        using ElementType = pointer_element_t<decltype(ptr)>;

        // Call default constructor for non-fundamental types
        if constexpr (std::is_fundamental_v<ElementType>)
        {
          std::fill(ptr, ptr + n, other);
        }
        else
        {
          std::for_each(ptr, ptr + n, [&other](auto& element) { new (std::addressof(element)) ElementType{other}; });
        }
      },
      buffers,
      std::forward<CTorArgTupleT>(ctor_arg_tuple));
  };

  /**
   * @brief Allocates memory to \c buffers and constructs those values
   */
  inline void allocate_and_construct(std::tuple<Ts*...>& buffers, const std::size_t n)
  {
    // Allocate new memory with specified capacity
    BasicMultiFieldArray::allocate(buffers, n);

    // Default construct new elements
    BasicMultiFieldArray::construct(buffers, n);
  };

  /**
   * @brief Allocates memory to \c buffers and constructs those values
   */
  template <typename CTorArgTupleT>
  inline void allocate_and_construct(std::tuple<Ts*...>& buffers, const std::size_t n, CTorArgTupleT&& ctor_arg_tuple)
  {
    // Allocate new memory with specified capacity
    BasicMultiFieldArray::allocate(buffers, n);

    // Value construct new elements
    BasicMultiFieldArray::construct(buffers, n, std::forward<CTorArgTupleT>(ctor_arg_tuple));
  };

  /**
   * @brief Deallocates memory from \c buffers
   */
  inline void deallocate(std::tuple<Ts*...>& buffers, const std::size_t n)
  {
    allocator_adapter_.deallocate(buffers, n);
    tuple_for_each([](auto& ptr) { ptr = nullptr; }, buffers);
  };

  /**
   * @brief Moves-constructor elements to \c buffers
   */
  inline void move_construct(std::tuple<Ts*...>& buffers, const std::size_t n)
  {
    tuple_for_each(
      [n](auto* dst_ptr, auto* src_ptr) {
        using ElementType = pointer_element_t<decltype(dst_ptr)>;

        if constexpr (std::is_fundamental_v<ElementType>)
        {
          std::memcpy(dst_ptr, src_ptr, sizeof(ElementType) * n);
        }
        else
        {
          const auto* const last_src_ptr = src_ptr + n;
          while (src_ptr != last_src_ptr)
          {
            new (dst_ptr) ElementType{std::move(*src_ptr)};
            ++src_ptr;
            ++dst_ptr;
          }
        }
      },
      buffers,
      data_);
  }

  /**
   * @brief Calls destructor on elements from \c buffers
   */
  inline void destroy(std::tuple<Ts*...>& buffers, const std::size_t n)
  {
    tuple_for_each(
      [n](auto& ptr) {
        using ElementType = pointer_element_t<decltype(ptr)>;

        if constexpr (!std::is_fundamental<ElementType>())
        {
          std::for_each(ptr, ptr + n, [](auto& element) { element.~ElementType(); });
        }
      },
      buffers);
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

    const std::size_t new_capacity = CapacityIncreasePolicy::next_capacity(capacity_);

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
  allocator_adapter_type allocator_adapter_;

  /// Pointers to data for each field
  std::tuple<Ts*...> data_;

  /// The effective number of elements in each component array
  std::size_t size_;

  /// The current capacity of each component array
  std::size_t capacity_;
};

/**
 * @brief Specifies a default capacity-growing policy through the method \c next_capacity
 */
struct DefaultCapacityIncreasePolicy
{
  static constexpr std::size_t next_capacity(std::size_t prev_capacity) { return 2 * prev_capacity + 2; };
};

/// Selects which allocator-adaptation strategy to use by default. If left unspecified, then
/// a single-pass allocation/de-allocation strategy is used, since it will be more efficient
/// in most cases.
#ifdef MF_DEFAULT_TO_PER_FIELD_ALLOCATION

template <typename... FieldTs> using default_allocator_adapter = multi_allocator_adapter<FieldTs...>;

#else

template <typename... FieldTs> using default_allocator_adapter = single_allocator_adapter<FieldTs...>;

#endif  // MF_DEFAULT_TO_SINGLE_PASS_ALLOCATOR

/**
 * @brief Convenience alias which uses \c std::allocator for each field
 */
template <typename... FieldTs>
using multi_field_array =
  BasicMultiFieldArray<std::tuple<FieldTs...>, default_allocator_adapter<FieldTs...>, DefaultCapacityIncreasePolicy>;

}  // namespace mf
