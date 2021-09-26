/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <string>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/mf.hpp>

TEST(MultiFieldArray, DefaultCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::size_t, std::allocator<std::size_t>> multi_field_array;

  ASSERT_TRUE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(multi_field_array.capacity(), 0UL);
}


TEST(MultiFieldArray, InitialSizeCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::size_t, std::allocator<std::size_t>> multi_field_array(
    10);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}


TEST(MultiFieldArray, RawPointerAccessByType)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::size_t, std::allocator<std::size_t>> multi_field_array(
    10);

  ASSERT_NE(multi_field_array.data<float>(), nullptr);
}


TEST(MultiFieldArray, RawPointerAccessByIndex)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::size_t, std::allocator<std::size_t>> multi_field_array(
    10);

  ASSERT_NE(multi_field_array.data<0>(), nullptr);
}


TEST(MultiFieldArray, CreateBackFromEmpty)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::size_t, std::allocator<std::size_t>> multi_field_array;

  const auto id = multi_field_array.create_back();

  ASSERT_EQ(id, 0UL);
  ASSERT_EQ(multi_field_array.size(), 1UL);
}


TEST(MultiFieldArray, EmplaceBackFromEmptyTrivial)
{
  mf::MultiFieldArray<mf::Fields<float, int, double>, std::size_t, std::allocator<std::size_t>> multi_field_array;

  multi_field_array.emplace_back(
    std::piecewise_construct, std::forward_as_tuple(0.f), std::forward_as_tuple(1), std::forward_as_tuple(2.0));

  ASSERT_EQ(multi_field_array.size(), 1UL);

  ASSERT_EQ(multi_field_array.at<float>(0), 0.0f);
  ASSERT_EQ(multi_field_array.at<int>(0), 1);
  ASSERT_EQ(multi_field_array.at<double>(0), 2.0);
}


TEST(MultiFieldArray, Access)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::size_t, std::allocator<std::size_t>> multi_field_array(
    10);

  const auto retval = multi_field_array.get<float, std::string>(3);

  (void)retval;
}
