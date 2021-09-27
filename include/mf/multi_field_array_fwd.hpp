/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <tuple>

namespace mf
{

template <typename... Ts> using Fields = std::tuple<Ts...>;

template <typename FieldTs> class MultiFieldArrayView;
template <typename FieldTs> class MultiFieldArrayIterator;
template <typename FieldTs, typename AllocatorT> class MultiFieldArray;

}  // namespace mf
