/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <memory>
#include <tuple>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/support/placement_new.hpp>

struct TestClass
{
  TestClass(int arg_1, float arg_2, std::unique_ptr<double> arg_3) : mem_1{arg_1}, mem_2{arg_2}, mem_3{std::move(arg_3)}
  {}
  TestClass(const TestClass&) = delete;

  int mem_1;
  float mem_2;
  std::unique_ptr<double> mem_3;
};

TEST(ApplyPlacementNew, CopyDeleted)
{
  std::aligned_storage_t<sizeof(TestClass)> buffer;

  mf::apply_placement_new<TestClass>(&buffer, std::forward_as_tuple(1, 5.f, std::make_unique<double>(3.0)));

  auto* const v_ptr = reinterpret_cast<TestClass*>(&buffer);

  ASSERT_EQ(v_ptr->mem_1, 1);
  ASSERT_EQ(v_ptr->mem_2, 5.f);
  ASSERT_TRUE(v_ptr->mem_3);

  v_ptr->~TestClass();
}
