/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <algorithm>
#include <iterator>
#include <string>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/multi_field_array.hpp>

TEST(MultiFieldArray, DefaultCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array;

  ASSERT_TRUE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(multi_field_array.capacity(), 0UL);
}


TEST(MultiFieldArray, InitialSizeCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, CopyCTor)
{
  // Create some existing container
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> original_multi_field_array(10);

  // Instance new container via copy constructor
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> copied_multi_field_array{
    original_multi_field_array};

  ASSERT_FALSE(copied_multi_field_array.empty());
  ASSERT_EQ(copied_multi_field_array.size(), 10UL);
  ASSERT_EQ(copied_multi_field_array.capacity(), 10UL);

  ASSERT_EQ(copied_multi_field_array.size(), original_multi_field_array.size());
  ASSERT_EQ(copied_multi_field_array.capacity(), original_multi_field_array.capacity());
}

TEST(MultiFieldArray, MoveCTor)
{
  // Create some existing container
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> original_multi_field_array(10);

  // Instance new container via move constructor
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> moved_multi_field_array{
    std::move(original_multi_field_array)};

  ASSERT_FALSE(moved_multi_field_array.empty());
  ASSERT_EQ(moved_multi_field_array.size(), 10UL);
  ASSERT_EQ(moved_multi_field_array.capacity(), 10UL);

  ASSERT_EQ(original_multi_field_array.size(), 0UL);
  ASSERT_EQ(original_multi_field_array.capacity(), 0UL);
}

TEST(MultiFieldArray, ResizeAfterDefaultCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array;

  multi_field_array.resize(10UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, ResizeLargerAfterInitialSizeCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  multi_field_array.resize(20UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 20UL);
  ASSERT_EQ(multi_field_array.capacity(), 20UL);
}

TEST(MultiFieldArray, ResizeSmallerAfterInitialSizeCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  multi_field_array.resize(5UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 5UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, ResizeSameSizeAfterInitialSizeCTor)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  multi_field_array.resize(10UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}


TEST(MultiFieldArray, RawPointerAccessByType)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  ASSERT_NE(multi_field_array.data<float>(), nullptr);
}


TEST(MultiFieldArray, RawPointerAccessByIndex)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  ASSERT_NE(multi_field_array.data<0>(), nullptr);
}


TEST(MultiFieldArray, EmplaceBackFromEmptyDefaultCTors)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array;

  multi_field_array.emplace_back();

  ASSERT_EQ(multi_field_array.size(), 1UL);
}


TEST(MultiFieldArray, EmplaceBackFromEmptyTrivial)
{
  mf::MultiFieldArray<mf::Fields<float, int, double>, std::allocator<std::size_t>> multi_field_array;

  multi_field_array.emplace_back(
    std::piecewise_construct, std::forward_as_tuple(0.f), std::forward_as_tuple(1), std::forward_as_tuple(2.0));

  ASSERT_EQ(multi_field_array.size(), 1UL);

  ASSERT_EQ(multi_field_array.get<float>(0), 0.0f);
  ASSERT_EQ(multi_field_array.get<int>(0), 1);
  ASSERT_EQ(multi_field_array.get<double>(0), 2.0);
}


TEST(MultiFieldArray, MutableMultiReferenceAccess)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  auto retval = multi_field_array.get<float, std::string>(3);

  std::get<0>(retval) = 10.f;
  std::get<1>(retval) = "hello there";

  ASSERT_EQ(multi_field_array.get<float>(3), 10.f);
  ASSERT_EQ(multi_field_array.get<std::string>(3), "hello there");
}


TEST(MultiFieldArray, MutableSingleReferenceAccess)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  auto& retval = multi_field_array.get<std::string>(3);

  retval = "hello there";

  ASSERT_EQ(multi_field_array.get<std::string>(3), "hello there");
}


TEST(MultiFieldArray, SingleFieldIteratorDistanceByType)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  ASSERT_EQ(std::distance(multi_field_array.begin<float>(), multi_field_array.end<float>()), multi_field_array.size());
  ASSERT_EQ(std::distance(multi_field_array.begin<int>(), multi_field_array.end<int>()), multi_field_array.size());
  ASSERT_EQ(
    std::distance(multi_field_array.begin<std::string>(), multi_field_array.end<std::string>()),
    multi_field_array.size());
}

TEST(MultiFieldArray, SingleFieldIteratorDistanceByIndex)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  ASSERT_EQ(std::distance(multi_field_array.begin<0>(), multi_field_array.end<0>()), multi_field_array.size());
  ASSERT_EQ(std::distance(multi_field_array.begin<1>(), multi_field_array.end<1>()), multi_field_array.size());
  ASSERT_EQ(std::distance(multi_field_array.begin<2>(), multi_field_array.end<2>()), multi_field_array.size());
}

TEST(MultiFieldArray, SingleFieldIteratorValueAssignmentByType)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  std::fill(multi_field_array.begin<int>(), multi_field_array.end<int>(), 3);

  std::for_each(multi_field_array.begin<int>(), multi_field_array.end<int>(), [](const int v) { ASSERT_EQ(v, 3); });
}

TEST(MultiFieldArray, SingleFieldIteratorValueAssignmentByIndex)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  std::fill(multi_field_array.begin<0>(), multi_field_array.end<0>(), 3);

  std::for_each(multi_field_array.begin<0>(), multi_field_array.end<0>(), [](const int v) { ASSERT_EQ(v, 3); });
}

TEST(MultiFieldArray, MultiFieldViewIterationValueAssignmentByType)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  for (auto [float_field, str_field] : multi_field_array.view<float, std::string>())
  {
    float_field = 3.f;
    str_field = "ok";
  }

  std::for_each(
    multi_field_array.begin<float>(), multi_field_array.end<float>(), [](const auto v) { ASSERT_EQ(v, 3.f); });

  std::for_each(multi_field_array.begin<std::string>(), multi_field_array.end<std::string>(), [](const auto& v) {
    ASSERT_EQ(v, "ok");
  });
}

TEST(MultiFieldArray, MultiFieldViewIterationValueAssignmentByIndex)
{
  mf::MultiFieldArray<mf::Fields<float, int, std::string>, std::allocator<std::size_t>> multi_field_array(10);

  for (auto [float_field, str_field] : multi_field_array.view<0, 2>())
  {
    float_field = 3.f;
    str_field = "ok";
  }

  std::for_each(multi_field_array.begin<0>(), multi_field_array.end<0>(), [](const auto v) { ASSERT_EQ(v, 3.f); });

  std::for_each(multi_field_array.begin<2>(), multi_field_array.end<2>(), [](const auto& v) { ASSERT_EQ(v, "ok"); });
}
