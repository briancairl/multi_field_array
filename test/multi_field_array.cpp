/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/multi_field_array.hpp>

TEST(MultiFieldArray, DefaultCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array;

  ASSERT_TRUE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(multi_field_array.capacity(), 0UL);
}


TEST(MultiFieldArray, InitialSizeCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, InitialSizeAndValueCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array{10, std::forward_as_tuple(4.f, 1, "bbb")};

  ASSERT_EQ(multi_field_array.size(), 10UL);

  for (std::size_t i = 0; i < multi_field_array.size(); ++i)
  {
    ASSERT_EQ(multi_field_array.get<float>(i), 4.f);
    ASSERT_EQ(multi_field_array.get<int>(i), 1);
    ASSERT_EQ(multi_field_array.get<std::string>(i), "bbb");
  }
}

TEST(MultiFieldArray, CopyCTor)
{
  // Create some existing container
  mf::multi_field_array<float, int, std::string> original_multi_field_array(10);

  // Instance new container via copy constructor
  mf::multi_field_array<float, int, std::string> copied_multi_field_array{original_multi_field_array};

  ASSERT_FALSE(copied_multi_field_array.empty());
  ASSERT_EQ(copied_multi_field_array.size(), 10UL);
  ASSERT_EQ(copied_multi_field_array.capacity(), 10UL);

  ASSERT_EQ(copied_multi_field_array.size(), original_multi_field_array.size());
  ASSERT_EQ(copied_multi_field_array.capacity(), original_multi_field_array.capacity());
}

TEST(MultiFieldArray, MoveCTor)
{
  // Create some existing container
  mf::multi_field_array<float, int, std::string> original_multi_field_array(10);

  // Instance new container via move constructor
  mf::multi_field_array<float, int, std::string> moved_multi_field_array{std::move(original_multi_field_array)};

  ASSERT_FALSE(moved_multi_field_array.empty());
  ASSERT_EQ(moved_multi_field_array.size(), 10UL);
  ASSERT_EQ(moved_multi_field_array.capacity(), 10UL);

  ASSERT_EQ(original_multi_field_array.size(), 0UL);
  ASSERT_EQ(original_multi_field_array.capacity(), 0UL);
}

TEST(MultiFieldArray, ResizeAfterDefaultCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array;

  multi_field_array.resize(10UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, ResizeLargerAfterInitialSizeCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  multi_field_array.resize(20UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 20UL);
  ASSERT_EQ(multi_field_array.capacity(), 20UL);
}

TEST(MultiFieldArray, ResizeSmallerAfterInitialSizeCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  multi_field_array.resize(5UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 5UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, ResizeSameSizeAfterInitialSizeCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  multi_field_array.resize(10UL);

  ASSERT_FALSE(multi_field_array.empty());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, ResizeSameWithInitialValues)
{
  mf::multi_field_array<float, int, std::string> multi_field_array;

  multi_field_array.resize(10UL, std::forward_as_tuple(4.f, 1, "bbb"));

  ASSERT_EQ(multi_field_array.size(), 10UL);

  for (std::size_t i = 0; i < multi_field_array.size(); ++i)
  {
    ASSERT_EQ(multi_field_array.get<float>(i), 4.f);
    ASSERT_EQ(multi_field_array.get<int>(i), 1);
    ASSERT_EQ(multi_field_array.get<std::string>(i), "bbb");
  }
}


TEST(MultiFieldArray, RawPointerAccessByType)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_NE(multi_field_array.data<float>(), nullptr);
}


TEST(MultiFieldArray, RawPointerAccessByIndex)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_NE(multi_field_array.data<0>(), nullptr);
}


TEST(MultiFieldArray, EmplaceBackFromEmptyDefaultCTors)
{
  mf::multi_field_array<float, int, std::string> multi_field_array;

  multi_field_array.emplace_back();

  ASSERT_EQ(multi_field_array.size(), 1UL);
}


TEST(MultiFieldArray, EmplaceBackFromCopyCTors)
{
  mf::multi_field_array<float, int, std::string> multi_field_array;

  multi_field_array.emplace_back(0.f, 1, std::string{"ok"});

  ASSERT_EQ(multi_field_array.size(), 1UL);

  ASSERT_EQ(multi_field_array.get<float>(0), 0.f);
  ASSERT_EQ(multi_field_array.get<int>(0), 1);
  ASSERT_EQ(multi_field_array.get<std::string>(0), "ok");
}


TEST(MultiFieldArray, EmplaceBackFromEmptyTrivial)
{
  mf::multi_field_array<float, int, double> multi_field_array;

  multi_field_array.emplace_back(
    std::piecewise_construct, std::forward_as_tuple(0.f), std::forward_as_tuple(1), std::forward_as_tuple(2.0));

  ASSERT_EQ(multi_field_array.size(), 1UL);

  ASSERT_EQ(multi_field_array.get<float>(0), 0.0f);
  ASSERT_EQ(multi_field_array.get<int>(0), 1);
  ASSERT_EQ(multi_field_array.get<double>(0), 2.0);
}


TEST(MultiFieldArray, MutableMultiReferenceAccess)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  auto retval = multi_field_array.get<float, std::string>(3);

  std::get<0>(retval) = 10.f;
  std::get<1>(retval) = "hello there";

  ASSERT_EQ(multi_field_array.get<float>(3), 10.f);
  ASSERT_EQ(multi_field_array.get<std::string>(3), "hello there");
}


TEST(MultiFieldArray, MutableSingleReferenceAccess)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  auto& retval = multi_field_array.get<std::string>(3);

  retval = "hello there";

  ASSERT_EQ(multi_field_array.get<std::string>(3), "hello there");
}


TEST(MultiFieldArray, SingleFieldIteratorDistanceByType)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_EQ(
    static_cast<std::size_t>(std::distance(multi_field_array.begin<float>(), multi_field_array.end<float>())),
    multi_field_array.size());
  ASSERT_EQ(
    static_cast<std::size_t>(std::distance(multi_field_array.begin<int>(), multi_field_array.end<int>())),
    multi_field_array.size());
  ASSERT_EQ(
    static_cast<std::size_t>(
      std::distance(multi_field_array.begin<std::string>(), multi_field_array.end<std::string>())),
    multi_field_array.size());
}

TEST(MultiFieldArray, SingleFieldIteratorDistanceByIndex)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_EQ(
    static_cast<std::size_t>(std::distance(multi_field_array.begin<0>(), multi_field_array.end<0>())),
    multi_field_array.size());
  ASSERT_EQ(
    static_cast<std::size_t>(std::distance(multi_field_array.begin<1>(), multi_field_array.end<1>())),
    multi_field_array.size());
  ASSERT_EQ(
    static_cast<std::size_t>(std::distance(multi_field_array.begin<2>(), multi_field_array.end<2>())),
    multi_field_array.size());
}

TEST(MultiFieldArray, SingleFieldIteratorValueAssignmentByType)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  std::fill(multi_field_array.begin<int>(), multi_field_array.end<int>(), 3);

  std::for_each(multi_field_array.begin<int>(), multi_field_array.end<int>(), [](const int v) { ASSERT_EQ(v, 3); });
}

TEST(MultiFieldArray, SingleFieldIteratorValueAssignmentByIndex)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  std::fill(multi_field_array.begin<0>(), multi_field_array.end<0>(), 3);

  std::for_each(multi_field_array.begin<0>(), multi_field_array.end<0>(), [](const int v) { ASSERT_EQ(v, 3); });
}

TEST(MultiFieldArray, MultiFieldViewIterationValueAssignmentByType)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_EQ(multi_field_array.view().size(), multi_field_array.size());

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
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_EQ(multi_field_array.view().size(), multi_field_array.size());

  for (auto [float_field, str_field] : multi_field_array.view<0, 2>())
  {
    float_field = 3.f;
    str_field = "ok";
  }

  std::for_each(multi_field_array.begin<0>(), multi_field_array.end<0>(), [](const auto v) { ASSERT_EQ(v, 3.f); });

  std::for_each(multi_field_array.begin<2>(), multi_field_array.end<2>(), [](const auto& v) { ASSERT_EQ(v, "ok"); });
}

TEST(MultiFieldArray, AllFieldViewIterationValueAssignment)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  ASSERT_EQ(multi_field_array.view().size(), multi_field_array.size());

  for (auto [float_field, int_field, str_field] : multi_field_array.view())
  {
    float_field = 3.f;
    int_field = 1;
    str_field = "ok";
  }

  std::for_each(multi_field_array.begin<0>(), multi_field_array.end<0>(), [](const float v) { ASSERT_EQ(v, 3.f); });

  std::for_each(multi_field_array.begin<1>(), multi_field_array.end<1>(), [](const int v) { ASSERT_EQ(v, 1); });

  std::for_each(
    multi_field_array.begin<2>(), multi_field_array.end<2>(), [](const std::string& v) { ASSERT_EQ(v, "ok"); });
}

TEST(MultiFieldArray, ConstMultiFieldViewIterationValueAssignmentByType)
{
  const mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  ASSERT_EQ(multi_field_array.view().size(), multi_field_array.size());

  for (auto [vec_field, str_field] : multi_field_array.view<std::vector<int>, std::string>())
  {
    ASSERT_TRUE(vec_field.empty());
    ASSERT_TRUE(str_field.empty());
  }
}

TEST(MultiFieldArray, ConstMultiFieldViewIterationValueAssignmentByIndex)
{
  const mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  ASSERT_EQ(multi_field_array.view().size(), multi_field_array.size());

  for (auto [vec_field, str_field] : multi_field_array.view<0, 1>())
  {
    ASSERT_TRUE(vec_field.empty());
    ASSERT_TRUE(str_field.empty());
  }
}

TEST(MultiFieldArray, ConstAllFieldViewIterationValueAssignment)
{
  const mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  ASSERT_EQ(multi_field_array.view().size(), multi_field_array.size());

  for (auto [vec_field, str_field] : multi_field_array.view())
  {
    ASSERT_TRUE(vec_field.empty());
    ASSERT_TRUE(str_field.empty());
  }
}

TEST(MultiFieldArray, DuplicateMultiFieldViewByIndex)
{
  const mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  for (auto [vec_field, vec_field_dup] : multi_field_array.view<0, 0>())
  {
    ASSERT_TRUE(vec_field.empty());
    ASSERT_TRUE(vec_field_dup.empty());
  }
}

TEST(MultiFieldArray, DuplicateMultiFieldViewByType)
{
  const mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  for (auto [vec_field, vec_field_dup] : multi_field_array.view<std::string, std::string>())
  {
    ASSERT_TRUE(vec_field.empty());
    ASSERT_TRUE(vec_field_dup.empty());
  }
}
