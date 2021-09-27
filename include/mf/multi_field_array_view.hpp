/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

// MF
#include <mf/multi_field_array_fwd.hpp>
#include <mf/support/tuple_dereference.hpp>
#include <mf/support/tuple_for_each.hpp>

namespace mf
{

/**
 * @brief Iterates over multiple fields of a MultiFieldArray
 */
template <typename... Ts> class MultiFieldArrayIterator<Fields<Ts...>>
{
public:
  inline bool operator==(const MultiFieldArrayIterator& other) const
  {
    return std::get<0>(this->ptr_) == std::get<0>(other.ptr_);
  }

  inline bool operator!=(const MultiFieldArrayIterator& other) const { return !this->operator==(other); }

  inline bool operator<(const MultiFieldArrayIterator& other) const
  {
    return std::get<0>(this->ptr_) < std::get<0>(other.ptr_);
  }

  inline bool operator>(const MultiFieldArrayIterator& other) const
  {
    return std::get<0>(this->ptr_) > std::get<0>(other.ptr_);
  }

  inline bool operator<=(const MultiFieldArrayIterator& other) const
  {
    return this->operator==(other) or this->operator<(other);
  }

  inline bool operator>=(const MultiFieldArrayIterator& other) const
  {
    return this->operator==(other) or this->operator>(other);
  }

  inline MultiFieldArrayIterator& operator=(const MultiFieldArrayIterator& other)
  {
    this->ptr_ = other.ptr_;
    return *this;
  }

  inline MultiFieldArrayIterator& operator++()
  {
    tuple_for_each(ptr_, [](auto& ptr) { ++ptr; });
    return *this;
  }

  inline MultiFieldArrayIterator operator++(int)
  {
    MultiFieldArrayIterator prev{*this};
    tuple_for_each(ptr_, [](auto& ptr) { ++ptr; });
    return prev;
  }

  inline auto operator*() { return tuple_dereference(ptr_); }

private:
  template <typename FieldTs> friend class MultiFieldArrayView;

  explicit MultiFieldArrayIterator(const std::tuple<Ts*...>& other_ptr) : ptr_{other_ptr} {}

  MultiFieldArrayIterator(const MultiFieldArrayIterator& other) : MultiFieldArrayIterator{other.ptr_} {}

  std::tuple<Ts*...> ptr_;
};

// TODO(enhancment) Need a version which works for const containers


/**
 * @brief Mutable view helper used for iterating over a subset of the field types of a MultiFieldArray
 */
template <typename... Ts> class MultiFieldArrayView<Fields<Ts...>>
{
public:
  inline MultiFieldArrayIterator<Fields<Ts...>> begin() { return MultiFieldArrayIterator<Fields<Ts...>>{data_}; }

  inline MultiFieldArrayIterator<Fields<Ts...>> end()
  {
    auto data_last = data_;
    tuple_for_each(data_last, [s = size_](auto& ptr) { ptr += s; });
    return MultiFieldArrayIterator<Fields<Ts...>>{data_last};
  }

private:
  template <typename FieldTs, typename AllocatorT> friend class MultiFieldArray;

  MultiFieldArrayView(const std::tuple<Ts*...>& data, const std::size_t size) : data_{data}, size_{size} {}

  /// Pointers to field data
  std::tuple<Ts*...> data_;

  /// Number of effective elements in each data buffer
  std::size_t size_;
};

}  // namespace mf
