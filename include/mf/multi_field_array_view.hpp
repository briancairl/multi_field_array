/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
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
template <typename... Ts> class MultiFieldArrayView<std::tuple<Ts...>>
{
public:
  inline ZipIterator<std::tuple<Ts*...>> begin() { return ZipIterator<std::tuple<Ts*...>>{data_}; }

  inline ZipIterator<std::tuple<Ts*...>> end()
  {
    auto data_last = data_;
    tuple_for_each(data_last, [s = size_](auto& ptr) { ptr += s; });
    return ZipIterator<std::tuple<Ts*...>>{data_last};
  }

  inline ZipIterator<std::tuple<const Ts*...>> begin() const { return ZipIterator<std::tuple<const Ts*...>>{data_}; }

  inline ZipIterator<std::tuple<const Ts*...>> end() const
  {
    auto data_last = data_;
    tuple_for_each(data_last, [s = size_](auto& ptr) { ptr += s; });
    return ZipIterator<std::tuple<const Ts*...>>{data_last};
  }

  inline std::size_t size() const { return size_; }

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

private:
  template <typename FieldTs, typename AllocatorTs> friend class BasicMultiFieldArray;

  MultiFieldArrayView(const std::tuple<Ts*...>& data, const std::size_t size) : data_{data}, size_{size} {}

  /// Pointers to field data
  std::tuple<Ts*...> data_;

  /// Number of effective elements in each data buffer
  std::size_t size_;
};

}  // namespace mf
