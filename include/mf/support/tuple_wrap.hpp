/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <tuple>

namespace mf
{

/**
 * @brief Creates \c std::tuple type with elements which have been wrapped with \c WrapperTmpl
 */
template <typename TupleT, template <typename> typename WrapperTmpl> struct tuple_wrap;

/**
 * @copydoc tuple_wrap
 */
template <typename... Ts, template <typename> typename WrapperTmpl> struct tuple_wrap<std::tuple<Ts...>, WrapperTmpl>
{
  using type = std::tuple<typename WrapperTmpl<Ts>::type...>;
};

}  // namespace mf
