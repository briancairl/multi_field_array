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
#include <mf/support/tuple_for_each.hpp>
#include <mf/support/tuple_ref.hpp>

namespace mf
{

/**
 * @brief Retrieves common \c iterator_category for a tuple of iterator-like types
 */
template <typename TupleOfIteratorsT> struct ZipIteratorCategory;

/**
 * @copydoc ZipIteratorCategory
 */
template <typename FirstIteratorT, typename SecondIteratorT, typename... OtherIteratorTs>
struct ZipIteratorCategory<std::tuple<FirstIteratorT, SecondIteratorT, OtherIteratorTs...>>
{
  using iterator_category = std::conditional_t<
    std::is_same_v<
      typename std::iterator_traits<FirstIteratorT>::iterator_category,
      typename std::iterator_traits<SecondIteratorT>::iterator_category>,
    typename ZipIteratorCategory<std::tuple<SecondIteratorT, OtherIteratorTs...>>::iterator_category,
    void  // cannot deduce common iterator_category
    >;
};

/**
 * @copydoc ZipIteratorCategory
 */
template <typename IteratorT> struct ZipIteratorCategory<std::tuple<IteratorT>>
{
  using iterator_category = typename std::iterator_traits<IteratorT>::iterator_category;
};

/**
 * @copydoc ZipIteratorCategory
 */
template <> struct ZipIteratorCategory<std::tuple<>>
{
  using iterator_category = void;
};

/**
 * @copydoc ZipIteratorCategory
 *
 *          Convenience alias
 */
template <typename TupleOfIteratorsT>
using zip_iterator_category_t = typename ZipIteratorCategory<TupleOfIteratorsT>::iterator_category;

/**
 * @brief Iterates over multiple iterators simultaneously
 */
template <typename FieldTs> class ZipIterator;

/**
 * @copydoc ZipIterator
 */
template <typename... IteratorTs> class ZipIterator<std::tuple<IteratorTs...>>
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

  explicit ZipIterator(const std::tuple<IteratorTs...>& other_ptr) : ptr_{other_ptr} {}

  ZipIterator(const ZipIterator& other) : ZipIterator{other.ptr_} {}

private:
  std::tuple<IteratorTs...> ptr_;
};

/**
 * @brief Creates a \c ZipIterator by deducing \c IteratorTs
 */
template <typename... IteratorTs>
inline ZipIterator<std::tuple<std::remove_reference_t<IteratorTs>...>> make_zip_iterator(IteratorTs... iterators)
{
  return ZipIterator<std::tuple<std::remove_reference_t<IteratorTs>...>>{std::forward_as_tuple(iterators...)};
}

}  // namespace mf

namespace std
{

/**
 * @brief Specialization of \c std::iterator_traits for a valid \c ZipIterator template instance
 */
template <typename TupleOfIteratorsT> struct iterator_traits<::mf::ZipIterator<TupleOfIteratorsT>>
{
  using difference_type = std::ptrdiff_t;
  using value_type = TupleOfIteratorsT;
  using pointer = ::mf::tuple_of_pointers_t<TupleOfIteratorsT>;
  using reference = ::mf::tuple_of_lvalue_references_t<TupleOfIteratorsT>;
  using iterator_category = ::mf::zip_iterator_category_t<TupleOfIteratorsT>;
};

}  // namespace std
