/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

// MF
#include <mf/multi_field_array_fwd.hpp>
#include <mf/support/assert.hpp>
#include <mf/support/tuple_for_each.hpp>
#include <mf/support/tuple_ref.hpp>
#include <mf/zip_iterator.hpp>

namespace mf
{

/**
 * @brief Mutable view helper used for iterating over a subset of the field types of a MultiFieldArray
 */
template <typename... Ts> class View<std::tuple<Ts...>>
{
public:
  /**
   * @brief Returns iterator to the first element represented in the view
   */
  inline ZipIterator<std::tuple<Ts*...>> begin() { return ZipIterator<std::tuple<Ts*...>>{data_}; }

  /**
   * @copydoc begin
   */
  inline ZipIterator<std::tuple<const Ts*...>> begin() const { return ZipIterator<std::tuple<const Ts*...>>{data_}; }

  /**
   * @brief Returns iterator to one element past the last element represented in the view
   */
  inline ZipIterator<std::tuple<Ts*...>> end()
  {
    auto data_last = data_;
    tuple_for_each(data_last, [s = size_](auto& ptr) { ptr += s; });
    return ZipIterator<std::tuple<Ts*...>>{data_last};
  }

  /**
   * @copydoc end
   */
  inline ZipIterator<std::tuple<const Ts*...>> end() const
  {
    auto data_last = data_;
    tuple_for_each(data_last, [s = size_](auto& ptr) { ptr += s; });
    return ZipIterator<std::tuple<const Ts*...>>{data_last};
  }

  /**
   * @brief Returns the number of elements represented in the view
   *
   *        i.e. \c std::distance(begin(), end()).
   */
  inline std::size_t size() const { return size_; }

  /**
   * @brief Returns true when element count is zero (view is empty)
   */
  inline bool empty() const { return size_ == 0; }

  /**
   * @brief Returns a reference to the element at specified location \c pos. No bounds checking is performed.
   *
   * @param pos  element position
   *
   * @return reference to element at \c pos
   */
  inline auto operator[](const std::size_t pos)
  {
    MF_ASSERT(pos >= 0 and pos < size_);
    auto data_at_pos = data_;
    tuple_for_each(data_at_pos, [pos](auto& ptr) { ptr += pos; });
    return tuple_dereference(data_at_pos);
  }

  /**
   * @copydoc operator[]
   */
  inline auto operator[](const std::size_t pos) const
  {
    MF_ASSERT(pos >= 0 and pos < size_);
    auto data_at_pos = data_;
    tuple_for_each(data_at_pos, [pos](auto& ptr) { ptr += pos; });
    return tuple_dereference(data_at_pos);
  }

  /**
   * @brief Returns a reference to the element at specified location \c pos. Bounds checking is performed.
   *
   * @param pos  element position
   *
   * @return reference to element at \c pos
   *
   * @throws \c std::out_of_range  if \c pos exceeds bounds of the view
   */
  inline auto at(const std::size_t pos)
  {
    if (pos >= size_)
    {
      throw std::out_of_range{"'pos' exceeds valid range of view"};
    }
    auto data_at_pos = data_;
    tuple_for_each(data_at_pos, [pos](auto& ptr) { ptr += pos; });
    return tuple_dereference(data_at_pos);
  }

  /**
   * @copydoc at
   */
  inline auto at(const std::size_t pos) const
  {
    if (pos >= size_)
    {
      throw std::out_of_range{"'pos' exceeds valid range of view"};
    }
    auto data_at_pos = data_;
    tuple_for_each(data_at_pos, [pos](auto& ptr) { ptr += pos; });
    return tuple_dereference(data_at_pos);
  }

private:
  template <typename FieldTs, typename AllocatorTs> friend class BasicMultiFieldArray;

  View(const std::tuple<Ts*...>& data, const std::size_t size) : data_{data}, size_{size} {}

  /// Pointers to field data
  std::tuple<Ts*...> data_;

  /// Number of effective elements in each data buffer
  std::size_t size_;
};

}  // namespace mf
