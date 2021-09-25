/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <tuple>
#include <utility>

namespace mf
{

template <typename... Ts> class Fields
{};

template <typename Fields, typename IndexT, typename AllocatorT> class MultiFieldArray;

template <typename... Ts, typename IndexT, typename AllocatorT> class MultiFieldArray<Fields<Ts...>, IndexT, AllocatorT>
{
public:
  MultiFieldArray() : size_{0}, capacity_{0}
  {
    detail::for_each(data_, [](auto& ptr) { data_ = nullptr; });
  }

  template <typename ValueT> inline ValueT& get(const IndexT index) { return std::get<ValueT*>(data_)[index]; }

  template <typename ValueT> inline const ValueT& get(const IndexT index) const
  {
    return std::get<ValueT*>(data_)[index];
  }

private:
  /// The effective number of elements in each component array
  std::size_t size_;

  /// The current capacity of each component array
  std::size_t capacity_;

  /// Pointers to data for each field
  std::tuple<Ts*...> data_;
};

template <typename... Ts, typename IndexT, typename AllocatorT>
struct MultiFieldArray<std::tuple<Ts...>, IndexT, AllocatorT>
    : public MultiFieldArray<Fields<Ts...>, IndexT, AllocatorT>
{
  template <typename... CTorArgTs>
  MultiFieldArray(CTorArgTs&&... args) :
      MultiFieldArray<Fields<Ts...>, IndexT, AllocatorT>{std::forward<CTorArgTs>(args)...} {};
};

}  // namespace mf
