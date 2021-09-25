/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <tuple>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/support/for_each.hpp>

TEST(ForEach, Unary)
{
  std::tuple<int*, float*, double*> some_pointers;

  std::size_t count = 0;

  mf::for_each(some_pointers, [&count](auto& ptr) {
    ++count;
    ptr = nullptr;
  });

  ASSERT_EQ(count, 3UL);

  mf::for_each(some_pointers, [](auto& ptr) { ASSERT_EQ(ptr, nullptr); });
}


TEST(ForEach, Binary)
{
  std::tuple<int*, float*, double*> some_pointers_lhs;

  mf::for_each(some_pointers_lhs, [](auto& ptr) { ptr = nullptr; });

  std::tuple<int*, float*, double*> some_pointers_rhs;

  std::size_t count = 0;

  mf::for_each(some_pointers_lhs, some_pointers_rhs, [&count](auto& lhs_ptr, auto& rhs_ptr) {
    ++count;
    rhs_ptr = lhs_ptr;
  });

  ASSERT_EQ(count, 3UL);

  mf::for_each(some_pointers_lhs, some_pointers_rhs, [](auto& lhs_ptr, auto& rhs_ptr) {
    ASSERT_EQ(lhs_ptr, nullptr);
    ASSERT_EQ(rhs_ptr, nullptr);
  });
}
