/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <memory>
#include <type_traits>

namespace mf
{

template <typename PointerT> struct pointer_element
{
  // Remove any referenced from type
  using cleaned_pointer_type = std::remove_reference_t<PointerT>;

  // Get element type that pointer would point to
  using type = typename std::pointer_traits<cleaned_pointer_type>::element_type;
};

template <typename PointerT> using pointer_element_t = typename pointer_element<PointerT>::type;

}  // namespace mf
