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
#include <mf/support/tuple_for_each.hpp>
#include <mf/support/tuple_ref.hpp>

namespace mf
{

/**
 * @brief Iterates over multiple fields of a MultiFieldArray
 */
template <typename... PtrTs> class ZipIterator<std::tuple<PtrTs...>>
{
public:
  inline bool operator==(const ZipIterator& other) const { return std::get<0>(this->ptr_) == std::get<0>(other.ptr_); }

  inline bool operator!=(const ZipIterator& other) const { return !this->operator==(other); }

  inline bool operator<(const ZipIterator& other) const { return std::get<0>(this->ptr_) < std::get<0>(other.ptr_); }

  inline bool operator>(const ZipIterator& other) const { return std::get<0>(this->ptr_) > std::get<0>(other.ptr_); }

  inline bool operator<=(const ZipIterator& other) const { return this->operator==(other) or this->operator<(other); }

  inline bool operator>=(const ZipIterator& other) const { return this->operator==(other) or this->operator>(other); }

  inline ZipIterator& operator=(const ZipIterator& other)
  {
    this->ptr_ = other.ptr_;
    return *this;
  }

  inline ZipIterator& operator--()
  {
    tuple_for_each(ptr_, [](auto& ptr) { --ptr; });
    return *this;
  }

  inline ZipIterator operator--(int)
  {
    ZipIterator prev{*this};
    tuple_for_each(ptr_, [](auto& ptr) { --ptr; });
    return prev;
  }

  inline ZipIterator& operator++()
  {
    tuple_for_each(ptr_, [](auto& ptr) { ++ptr; });
    return *this;
  }

  inline ZipIterator operator++(int)
  {
    ZipIterator prev{*this};
    tuple_for_each(ptr_, [](auto& ptr) { ++ptr; });
    return prev;
  }

  inline auto operator*() { return tuple_dereference(ptr_); }

  inline auto operator*() const { return tuple_dereference(ptr_); }

  explicit ZipIterator(const std::tuple<PtrTs...>& other_ptr) : ptr_{other_ptr} {}

  ZipIterator(const ZipIterator& other) : ZipIterator{other.ptr_} {}

private:
  std::tuple<PtrTs...> ptr_;
};

template <typename... IteratorTs>
inline ZipIterator<std::tuple<std::remove_reference_t<IteratorTs>...>> make_zip_iterator(IteratorTs... iterators)
{
  return ZipIterator<std::tuple<std::remove_reference_t<IteratorTs>...>>{std::forward_as_tuple(iterators...)};
}

}  // namespace mf

namespace std
{

template <typename ValueTupleT> struct iterator_traits<::mf::ZipIterator<ValueTupleT>>
{
  using difference_type = std::ptrdiff_t;
  using value_type = ValueTupleT;
  using pointer = ::mf::tuple_of_pointers_t<ValueTupleT>;
  using reference = ::mf::tuple_of_lvalue_references_t<ValueTupleT>;
  using iterator_category = std::random_access_iterator_tag;
};

}  // namespace std
