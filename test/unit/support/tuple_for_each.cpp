/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <tuple>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/support/tuple_for_each.hpp>

TEST(TupleForEach, Unary)
{
  std::tuple<int*, float*, double*> some_pointers;

  std::size_t count = 0;

  mf::tuple_for_each(
    [&count](auto& ptr) {
      ++count;
      ptr = nullptr;
    },
    some_pointers);

  ASSERT_EQ(count, 3UL);

  mf::tuple_for_each([](auto& ptr) { ASSERT_EQ(ptr, nullptr); }, some_pointers);
}


TEST(TupleForEach, Binary)
{
  std::tuple<int*, float*, double*> some_pointers_lhs;

  mf::tuple_for_each([](auto& ptr) { ptr = nullptr; }, some_pointers_lhs);

  std::tuple<int*, float*, double*> some_pointers_rhs;

  std::size_t count = 0;

  mf::tuple_for_each(
    [&count](auto& lhs_ptr, auto& rhs_ptr) {
      ++count;
      rhs_ptr = lhs_ptr;
    },
    some_pointers_lhs,
    some_pointers_rhs);

  ASSERT_EQ(count, 3UL);

  mf::tuple_for_each(
    [](auto& lhs_ptr, auto& rhs_ptr) {
      ASSERT_EQ(lhs_ptr, nullptr);
      ASSERT_EQ(rhs_ptr, nullptr);
    },
    some_pointers_lhs,
    some_pointers_rhs);
}

TEST(TupleForEach, Nary)
{
  std::tuple<int*, float*, double*> some_pointers;

  mf::tuple_for_each(
    [](auto& ptr0, auto& ptr1, auto& ptr2, auto& ptr3) {
      ptr0 = nullptr;
      ASSERT_EQ(ptr1, nullptr);
      ASSERT_EQ(ptr2, nullptr);
      ASSERT_EQ(ptr3, nullptr);
    },
    some_pointers,
    some_pointers,
    some_pointers,
    some_pointers);
}
