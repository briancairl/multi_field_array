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

TEST(MultiFieldArray, ReserveMoreAfterDefaultCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array;

  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(multi_field_array.capacity(), 0UL);

  multi_field_array.reserve(10UL);

  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, ReserveMoreAfterInitialSizeCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  const auto* const prev_data_ptr = multi_field_array.data<float>();
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);

  multi_field_array.reserve(20UL);

  ASSERT_NE(prev_data_ptr, multi_field_array.data<float>());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 20UL);
}

TEST(MultiFieldArray, ReserveSameAfterInitialSizeCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  const auto* const prev_data_ptr = multi_field_array.data<float>();
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);

  multi_field_array.reserve(10UL);

  ASSERT_EQ(prev_data_ptr, multi_field_array.data<float>());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, ReserveLessAfterInitialSizeCTor)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  const auto* const prev_data_ptr = multi_field_array.data<float>();
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);

  multi_field_array.reserve(1UL);

  ASSERT_EQ(prev_data_ptr, multi_field_array.data<float>());
  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
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

TEST(MultiFieldArray, SingleFieldExplicitConstIteratorAccessByType)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  std::for_each(
    multi_field_array.cbegin<std::string>(), multi_field_array.cend<std::string>(), [](const std::string& v) {
      ASSERT_TRUE(v.empty());
    });
}

TEST(MultiFieldArray, SingleFieldExplicitConstValueAccessByIndex)
{
  mf::multi_field_array<float, int, std::string> multi_field_array(10);

  std::for_each(
    multi_field_array.cbegin<2>(), multi_field_array.cend<2>(), [](const std::string& v) { ASSERT_TRUE(v.empty()); });
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


TEST(MultiFieldArray, AllFieldIteration)
{
  mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  for (auto [vec_field, str_field] : multi_field_array)
  {
    ASSERT_TRUE(std::is_reference_v<decltype(vec_field)>);
    ASSERT_TRUE(std::is_reference_v<decltype(str_field)>);
    ASSERT_TRUE(vec_field.empty());
    ASSERT_TRUE(str_field.empty());
  }
}

TEST(MultiFieldArray, AllFieldIterationConst)
{
  const mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  for (auto [vec_field, str_field] : multi_field_array)
  {
    ASSERT_TRUE(std::is_reference_v<decltype(vec_field)>);
    ASSERT_TRUE(std::is_reference_v<decltype(str_field)>);
    ASSERT_TRUE(vec_field.empty());
    ASSERT_TRUE(str_field.empty());
  }
}

TEST(MultiFieldArray, AllFieldIterationExplicitConst)
{
  mf::multi_field_array<std::vector<int>, std::string> multi_field_array(10);

  std::for_each(multi_field_array.cbegin(), multi_field_array.cend(), [](const auto& tuple_v) {
    using TupleOfReferencesT = std::remove_reference_t<decltype(tuple_v)>;
    ASSERT_TRUE((std::is_reference_v<std::tuple_element_t<0, TupleOfReferencesT>>));
    ASSERT_TRUE((std::is_const_v<std::remove_reference_t<std::tuple_element_t<0, TupleOfReferencesT>>>));
    ASSERT_TRUE((std::is_reference_v<std::tuple_element_t<1, TupleOfReferencesT>>));
    ASSERT_TRUE((std::is_const_v<std::remove_reference_t<std::tuple_element_t<1, TupleOfReferencesT>>>));
  });
}

TEST(MultiFieldArray, ViewPositionalAccess)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  const auto [int_ref, str_ref] = multi_field_array.view<int, std::string>()[4];

  ASSERT_TRUE(std::is_reference_v<decltype(int_ref)>);
  ASSERT_TRUE(std::is_reference_v<decltype(str_ref)>);
  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, ViewPositionalAssignment)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  multi_field_array.view<int, std::string>()[4] = std::forward_as_tuple(1, "unacceptable!");

  const auto [int_ref, str_ref] = multi_field_array.view<int, std::string>()[4];

  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, ConstViewPositionalAccess)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);
  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  mf::multi_field_array<int, std::string> const_multi_field_array{multi_field_array};

  const auto [int_ref, str_ref] = const_multi_field_array.view<int, std::string>()[4];

  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, ViewPositionalAccessWithBoundsCheckValid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  const auto [int_ref, str_ref] = multi_field_array.view<int, std::string>().at(4);

  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, ConstViewPositionalAccessWithBoundsCheckValid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);
  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  mf::multi_field_array<int, std::string> const_multi_field_array{multi_field_array};

  const auto [int_ref, str_ref] = const_multi_field_array.view<int, std::string>().at(4);

  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, ViewPositionalAccessWithBoundsCheckInvalid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  ASSERT_THROW((multi_field_array.view<int, std::string>().at(40)), std::out_of_range);
}

TEST(MultiFieldArray, ConstViewPositionalAccessWithBoundsCheckInvalid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);
  const mf::multi_field_array<int, std::string> const_multi_field_array{multi_field_array};

  ASSERT_THROW((const_multi_field_array.view<int, std::string>().at(40)), std::out_of_range);
}

TEST(MultiFieldArray, AllFieldPositionalAccess)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  const auto [int_ref, str_ref] = multi_field_array[4];

  ASSERT_TRUE(std::is_reference_v<decltype(int_ref)>);
  ASSERT_TRUE(std::is_reference_v<decltype(str_ref)>);
  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, AllFieldPositionalAssignment)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  multi_field_array[4] = std::forward_as_tuple(1, "unacceptable!");

  const auto [int_ref, str_ref] = multi_field_array[4];

  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, ConstAllFieldPositionalAccess)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);
  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  const mf::multi_field_array<int, std::string> const_multi_field_array{multi_field_array};

  const auto [int_ref, str_ref] = const_multi_field_array[4];

  ASSERT_TRUE(std::is_reference_v<decltype(int_ref)>);
  ASSERT_TRUE(std::is_reference_v<decltype(str_ref)>);
  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, AllFieldPositionalAccessWithBoundsCheckValid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  const auto [int_ref, str_ref] = multi_field_array.at(4);

  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, ConstAllFieldPositionalAccessWithBoundsCheckValid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);
  multi_field_array.get<int>(4) = 1;
  multi_field_array.get<std::string>(4) = "unacceptable!";

  const mf::multi_field_array<int, std::string> const_multi_field_array{multi_field_array};

  const auto [int_ref, str_ref] = const_multi_field_array.at(4);

  ASSERT_EQ(int_ref, 1);
  ASSERT_EQ(str_ref, "unacceptable!");
}

TEST(MultiFieldArray, AllFieldPositionalAccessWithBoundsCheckInvalid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);

  ASSERT_THROW(multi_field_array.at(40), std::out_of_range);
}

TEST(MultiFieldArray, ConstAllFieldPositionalAccessWithBoundsCheckInvalid)
{
  mf::multi_field_array<int, std::string> multi_field_array(10);
  const mf::multi_field_array<int, std::string> const_multi_field_array{multi_field_array};

  ASSERT_THROW(const_multi_field_array.at(40), std::out_of_range);
}

TEST(MultiFieldArray, CustomCapacityIncreasePolicy)
{
  struct CustomPolicy
  {
    static constexpr std::size_t next_capacity(const std::size_t prev_capacity) { return prev_capacity * 4UL + 10UL; }
  };

  using multi_field_array_type =
    mf::BasicMultiFieldArray<std::tuple<int>, mf::default_allocator_adapter<int>, CustomPolicy>;

  multi_field_array_type array;

  ASSERT_EQ(array.capacity(), 0UL);

  array.emplace_back(1);

  ASSERT_EQ(array.capacity(), CustomPolicy::next_capacity(1));

  // Add just enough elements to trigger reallocation
  const std::size_t n_to_trigger_reallocation = array.capacity();
  for (std::size_t i = 0; i < n_to_trigger_reallocation; ++i)
  {
    array.emplace_back(static_cast<int>(i + 2));
  }

  ASSERT_GT(array.capacity(), n_to_trigger_reallocation);
  ASSERT_EQ(array.capacity(), CustomPolicy::next_capacity(n_to_trigger_reallocation + 1));
}

TEST(MultiFieldArray, Clear)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);

  multi_field_array.clear();

  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);
}

TEST(MultiFieldArray, Release)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  ASSERT_EQ(multi_field_array.size(), 10UL);
  ASSERT_EQ(multi_field_array.capacity(), 10UL);

  multi_field_array.release();

  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(multi_field_array.capacity(), 0UL);
}

TEST(MultiFieldArray, IteratorDistance)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  ASSERT_EQ(
    static_cast<std::size_t>(std::distance(multi_field_array.begin(), multi_field_array.end())),
    multi_field_array.size());
}

TEST(MultiFieldArray, InsertCountMiddle)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  multi_field_array.insert(std::next(multi_field_array.begin(), 5), 5, std::forward_as_tuple(9, "unacceptable!"));

  ASSERT_EQ(multi_field_array.size(), 15UL);

  for (std::size_t i = 0; i < 5; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }

  for (std::size_t i = 5; i < 10; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 9);
    ASSERT_EQ(vstring, "unacceptable!");
  }

  for (std::size_t i = 10; i < 15; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, InsertCountEnd)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  multi_field_array.insert(multi_field_array.end(), 5, std::forward_as_tuple(9, "unacceptable!"));

  ASSERT_EQ(multi_field_array.size(), 15UL);

  for (std::size_t i = 0; i < 10; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }

  for (std::size_t i = 10; i < 15; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 9);
    ASSERT_EQ(vstring, "unacceptable!");
  }
}

TEST(MultiFieldArray, InsertCountBegin)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  multi_field_array.insert(multi_field_array.begin(), 5, std::forward_as_tuple(9, "unacceptable!"));

  ASSERT_EQ(multi_field_array.size(), 15UL);

  for (std::size_t i = 0; i < 5; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 9);
    ASSERT_EQ(vstring, "unacceptable!");
  }

  for (std::size_t i = 5; i < 15; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, InsertCountWithIndex)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto pos = multi_field_array.insert(0, 5, std::forward_as_tuple(9, "unacceptable!"));
  ASSERT_EQ(pos, 0UL);

  ASSERT_EQ(multi_field_array.size(), 15UL);

  for (std::size_t i = 0; i < 5; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 9);
    ASSERT_EQ(vstring, "unacceptable!");
  }

  for (std::size_t i = 5; i < 15; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, InsertNone)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto pos = multi_field_array.insert(5, 0, std::forward_as_tuple(9, "unacceptable!"));
  ASSERT_EQ(pos, 5UL);

  ASSERT_EQ(multi_field_array.size(), 10UL);

  for (std::size_t i = 0; i < 10; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}


TEST(MultiFieldArray, InsertSingle)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto pos = multi_field_array.insert(0, std::forward_as_tuple(9, "unacceptable!"));
  ASSERT_EQ(pos, 0UL);

  ASSERT_EQ(multi_field_array.size(), 11UL);

  for (std::size_t i = 0; i < 1; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 9);
    ASSERT_EQ(vstring, "unacceptable!");
  }

  for (std::size_t i = 1; i < 11; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseMiddle)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto itr = multi_field_array.erase(std::next(multi_field_array.begin(), 5));
  ASSERT_EQ(itr, std::next(multi_field_array.begin(), 5));

  ASSERT_EQ(multi_field_array.size(), 9UL);

  for (std::size_t i = 0; i < 9; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseLast)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto itr = multi_field_array.erase(std::next(multi_field_array.begin(), 9));
  ASSERT_EQ(itr, multi_field_array.end());

  ASSERT_EQ(multi_field_array.size(), 9UL);

  for (std::size_t i = 0; i < 9; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseFirst)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto itr = multi_field_array.erase(multi_field_array.begin());
  ASSERT_EQ(itr, multi_field_array.begin());

  ASSERT_EQ(multi_field_array.size(), 9UL);

  for (std::size_t i = 0; i < 9; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseIndex)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto pos = multi_field_array.erase(5);
  ASSERT_EQ(pos, 5UL);

  ASSERT_EQ(multi_field_array.size(), 9UL);

  for (std::size_t i = 0; i < 9; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseRangeMiddle)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto itr =
    multi_field_array.erase(std::next(multi_field_array.begin(), 5), std::next(multi_field_array.begin(), 8));
  ASSERT_EQ(itr, std::next(multi_field_array.begin(), 5));

  ASSERT_EQ(multi_field_array.size(), 7UL);

  for (std::size_t i = 0; i < 7; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseRangeEnd)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto itr = multi_field_array.erase(std::next(multi_field_array.begin(), 5), multi_field_array.end());
  ASSERT_EQ(itr, multi_field_array.end());

  ASSERT_EQ(multi_field_array.size(), 5UL);

  for (std::size_t i = 0; i < 5; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseRangeBegin)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto itr = multi_field_array.erase(multi_field_array.begin(), std::prev(multi_field_array.end(), 5));
  ASSERT_EQ(itr, multi_field_array.begin());

  ASSERT_EQ(multi_field_array.size(), 5UL);

  for (std::size_t i = 0; i < 5; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, EraseRangeIndex)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  const auto pos = multi_field_array.erase(0UL, 5UL);
  ASSERT_EQ(pos, 0UL);

  ASSERT_EQ(multi_field_array.size(), 5UL);

  for (std::size_t i = 0; i < 5; ++i)
  {
    const auto [vint, vstring] = multi_field_array.get<int, std::string>(i);
    ASSERT_EQ(vint, 1);
    ASSERT_EQ(vstring, "ok!");
  }
}

TEST(MultiFieldArray, PopBack)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  ASSERT_EQ(multi_field_array.size(), 10UL);

  multi_field_array.pop_back();

  ASSERT_EQ(multi_field_array.size(), 9UL);
}

TEST(MultiFieldArray, DereferenceMutableItr)
{
  mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};

  auto itr = std::prev(multi_field_array.end());

  auto [i, s] = (*itr);

  ASSERT_EQ(i, 1);
  ASSERT_EQ(s, "ok!");
}

TEST(MultiFieldArray, DereferenceImmutableItr)
{
  const mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};

  auto itr = std::prev(multi_field_array.end());

  auto [i, s] = (*itr);

  ASSERT_EQ(i, 1);
  ASSERT_EQ(s, "ok!");
}

TEST(MultiFieldArray, MutableFrontAccessSingle)
{
  mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  ASSERT_EQ(multi_field_array.front<int>(), 1);
  ASSERT_EQ(multi_field_array.front<1>(), "ok!");
}

TEST(MultiFieldArray, ImmutableFrontAccessSingle)
{
  const mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  ASSERT_EQ(multi_field_array.front<int>(), 1);
  ASSERT_EQ(multi_field_array.front<1>(), "ok!");
}

TEST(MultiFieldArray, MutableFrontAccessAll)
{
  mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  auto [i, s] = multi_field_array.front();
  ASSERT_EQ(i, 1);
  ASSERT_EQ(s, "ok!");
}

TEST(MultiFieldArray, ImmutableFrontAccessAll)
{
  const mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  const auto [i, s] = multi_field_array.front();
  ASSERT_EQ(i, 1);
  ASSERT_EQ(s, "ok!");
}

TEST(MultiFieldArray, MutableBackAccessSingle)
{
  mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  ASSERT_EQ(multi_field_array.back<int>(), 1);
  ASSERT_EQ(multi_field_array.back<1>(), "ok!");
}

TEST(MultiFieldArray, ImmutableBackAccessSingle)
{
  const mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  ASSERT_EQ(multi_field_array.back<int>(), 1);
  ASSERT_EQ(multi_field_array.back<1>(), "ok!");
}

TEST(MultiFieldArray, MutableBackAccessAll)
{
  mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  auto [i, s] = multi_field_array.back();
  ASSERT_EQ(i, 1);
  ASSERT_EQ(s, "ok!");
}

TEST(MultiFieldArray, ImmutableBackAccessAll)
{
  const mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  const auto [i, s] = multi_field_array.back();
  ASSERT_EQ(i, 1);
  ASSERT_EQ(s, "ok!");
}

TEST(MultiFieldArray, CopyAssignment)
{
  const mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  mf::multi_field_array<int, std::string> assign_to;
  ASSERT_EQ(assign_to.size(), 0UL);

  assign_to = multi_field_array;

  ASSERT_EQ(assign_to.size(), 10UL);

  ASSERT_TRUE(std::equal(assign_to.begin(), assign_to.end(), multi_field_array.begin()));
}

TEST(MultiFieldArray, MoveAssignment)
{
  mf::multi_field_array<int, std::string> multi_field_array{10, std::forward_as_tuple(1, "ok!")};
  mf::multi_field_array<int, std::string> assign_to;
  ASSERT_EQ(assign_to.size(), 0UL);

  assign_to = std::move(multi_field_array);

  ASSERT_EQ(multi_field_array.size(), 0UL);
  ASSERT_EQ(assign_to.size(), 10UL);

  for (const auto [i, s] : assign_to)
  {
    ASSERT_EQ(i, 1);
    ASSERT_EQ(s, "ok!");
  }
}

TEST(MultiFieldArray, ReverseIteratorDistance)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10, std::forward_as_tuple(1, "ok!"));

  ASSERT_EQ(
    static_cast<std::size_t>(std::distance(multi_field_array.rbegin(), multi_field_array.rend())),
    multi_field_array.size());
}

TEST(MultiFieldArray, ReverseIteratorMulti)
{
  mf::multi_field_array<int, std::string> multi_field_array;

  for (int i = 0; i < 10; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  auto ritr = multi_field_array.rbegin();
  for (int i = 0; i < 10; ++i, ++ritr)
  {
    const auto [gi, gs] = multi_field_array.get<int, std::string>(10 - i - 1);
    const auto [ri, rs] = (*ritr);
    ASSERT_EQ(gi, ri);
    ASSERT_EQ(gs, rs);
  }
}

TEST(MultiFieldArray, ConstReverseIteratorMulti)
{
  mf::multi_field_array<int, std::string> multi_field_array;

  for (int i = 0; i < 10; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  auto ritr = multi_field_array.crbegin();
  for (int i = 0; i < 10; ++i, ++ritr)
  {
    const auto [gi, gs] = multi_field_array.get<int, std::string>(10 - i - 1);
    const auto [ri, rs] = (*ritr);
    ASSERT_EQ(gi, ri);
    ASSERT_EQ(gs, rs);
  }

  ASSERT_TRUE(std::equal(multi_field_array.rbegin(), multi_field_array.rend(), multi_field_array.crbegin()));
  ASSERT_TRUE(std::equal(multi_field_array.crbegin(), multi_field_array.crend(), multi_field_array.rbegin()));
}

TEST(MultiFieldArray, ReverseIteratorSingleTyped)
{
  mf::multi_field_array<int, std::string> multi_field_array;

  for (int i = 0; i < 10; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  auto ritr = multi_field_array.rbegin<int>();
  for (int i = 0; i < 10; ++i, ++ritr)
  {
    const auto gi = multi_field_array.get<int>(10 - i - 1);
    ASSERT_EQ(gi, *ritr);
  }
}

TEST(MultiFieldArray, ConstReverseIteratorSingleTyped)
{
  mf::multi_field_array<int, std::string> multi_field_array;

  for (int i = 0; i < 10; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  auto ritr = multi_field_array.crbegin<int>();
  for (int i = 0; i < 10; ++i, ++ritr)
  {
    const auto gi = multi_field_array.get<int>(10 - i - 1);
    ASSERT_EQ(gi, *ritr);
  }

  ASSERT_TRUE(
    std::equal(multi_field_array.rbegin<int>(), multi_field_array.rend<int>(), multi_field_array.crbegin<int>()));
  ASSERT_TRUE(
    std::equal(multi_field_array.crbegin<int>(), multi_field_array.crend<int>(), multi_field_array.rbegin<int>()));
}

TEST(MultiFieldArray, ReverseIteratorSingleIndex)
{
  mf::multi_field_array<int, std::string> multi_field_array;

  for (int i = 0; i < 10; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  auto ritr = multi_field_array.rbegin<0>();
  for (int i = 0; i < 10; ++i, ++ritr)
  {
    const auto gi = multi_field_array.get<int>(10 - i - 1);
    ASSERT_EQ(gi, *ritr);
  }
}

TEST(MultiFieldArray, ConstReverseIteratorSingleIndex)
{
  mf::multi_field_array<int, std::string> multi_field_array;

  for (int i = 0; i < 10; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  auto ritr = multi_field_array.crbegin<0>();
  for (int i = 0; i < 10; ++i, ++ritr)
  {
    const auto gi = multi_field_array.get<int>(10 - i - 1);
    ASSERT_EQ(gi, *ritr);
  }

  ASSERT_TRUE(std::equal(multi_field_array.rbegin<0>(), multi_field_array.rend<0>(), multi_field_array.crbegin<0>()));
  ASSERT_TRUE(std::equal(multi_field_array.crbegin<0>(), multi_field_array.crend<0>(), multi_field_array.rbegin<0>()));
}

TEST(MultiFieldArray, ReverseIteratorAssignment)
{
  mf::multi_field_array<int, std::string> multi_field_array{10};

  std::for_each(multi_field_array.rbegin(), multi_field_array.rend(), [](auto tup) {
    std::get<0>(tup) = 9;
    std::get<1>(tup) = "ok!";
  });

  std::for_each(multi_field_array.rbegin(), multi_field_array.rend(), [](auto tup) {
    ASSERT_EQ(std::get<0>(tup), 9);
    ASSERT_EQ(std::get<1>(tup), "ok!");
  });
}

TEST(MultiFieldArray, ReverseIteratorArrowOperator)
{
  mf::multi_field_array<int, std::string> multi_field_array;

  for (int i = 0; i < 11; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  ASSERT_EQ(std::prev(multi_field_array.rend<std::string>())->size(), 1UL)
    << *std::prev(multi_field_array.rend<std::string>());
  ASSERT_EQ(multi_field_array.rbegin<std::string>()->size(), 2UL) << *multi_field_array.rbegin<std::string>();
}

TEST(MultiFieldArray, DirectPointAccess)
{
  mf::multi_field_array<int, std::string> multi_field_array;
  multi_field_array.resize(10);

  auto data_ptr_tuple = multi_field_array.data();

  ASSERT_EQ(std::get<0>(data_ptr_tuple), multi_field_array.data<0>());
  ASSERT_EQ(std::get<1>(data_ptr_tuple), multi_field_array.data<1>());
}

TEST(MultiFieldArray, ConstDirectPointAccess)
{
  const mf::multi_field_array<int, std::string> multi_field_array{10};

  const auto data_ptr_tuple = multi_field_array.data();

  ASSERT_EQ(std::get<0>(data_ptr_tuple), multi_field_array.data<0>());
  ASSERT_EQ(std::get<1>(data_ptr_tuple), multi_field_array.data<1>());
}

TEST(MultiFieldArray, PushBackCopy)
{
  mf::multi_field_array<int, std::string> multi_field_array{10};

  for (int i = 0; i < 11; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  mf::multi_field_array<int, std::string> multi_field_array_copy_target;

  for (const auto& v : multi_field_array)
  {
    multi_field_array_copy_target.push_back(v);
  }

  ASSERT_EQ(multi_field_array.size(), multi_field_array_copy_target.size());

  ASSERT_TRUE(std::equal(
    multi_field_array.begin<int>(), multi_field_array.end<int>(), multi_field_array_copy_target.cbegin<int>()));

  ASSERT_TRUE(std::equal(
    multi_field_array.begin<std::string>(),
    multi_field_array.end<std::string>(),
    multi_field_array_copy_target.cbegin<std::string>()));
}

TEST(MultiFieldArray, BackInserterCopy)
{
  mf::multi_field_array<int, std::string> multi_field_array{10};

  for (int i = 0; i < 11; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  mf::multi_field_array<int, std::string> multi_field_array_copy_target;

  std::copy(multi_field_array.begin(), multi_field_array.end(), std::back_inserter(multi_field_array_copy_target));

  ASSERT_EQ(multi_field_array.size(), multi_field_array_copy_target.size());

  ASSERT_TRUE(std::equal(
    multi_field_array.begin<int>(), multi_field_array.end<int>(), multi_field_array_copy_target.cbegin<int>()));

  ASSERT_TRUE(std::equal(
    multi_field_array.begin<std::string>(),
    multi_field_array.end<std::string>(),
    multi_field_array_copy_target.cbegin<std::string>()));
}

TEST(MultiFieldArray, Swap)
{
  mf::multi_field_array<int, std::string> multi_field_array{10};

  for (int i = 0; i < 11; ++i)
  {
    multi_field_array.emplace_back(i, std::to_string(i));
  }

  const auto expected_pointer_values = multi_field_array.data();
  const auto multi_field_array_swap_expected{multi_field_array};

  mf::multi_field_array<int, std::string> multi_field_array_swap_target;
  multi_field_array_swap_target.swap(multi_field_array);

  ASSERT_EQ(expected_pointer_values, multi_field_array_swap_target.data());

  ASSERT_EQ(multi_field_array_swap_expected.size(), multi_field_array_swap_target.size());

  ASSERT_TRUE(std::equal(
    multi_field_array_swap_expected.begin<int>(),
    multi_field_array_swap_expected.end<int>(),
    multi_field_array_swap_target.cbegin<int>()));

  ASSERT_TRUE(std::equal(
    multi_field_array_swap_expected.begin<std::string>(),
    multi_field_array_swap_expected.end<std::string>(),
    multi_field_array_swap_target.cbegin<std::string>()));
}

#include <memory>  // std::unique_ptr
#include <mutex>  // std::mutex

TEST(MultiFieldArray, MoveOnlyType)
{
  mf::multi_field_array<int, std::unique_ptr<std::mutex>> move_only_sequence;
  move_only_sequence.reserve(4);

  ASSERT_EQ(move_only_sequence.size(), 0UL);
  ASSERT_EQ(move_only_sequence.capacity(), 4UL);

  move_only_sequence.resize(4);
  ASSERT_EQ(move_only_sequence.size(), 4UL);
  ASSERT_EQ(move_only_sequence.capacity(), 4UL);
}