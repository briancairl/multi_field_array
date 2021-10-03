/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <iterator>
#include <string>
#include <vector>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/zip_iterator.hpp>

TEST(MultiFieldArray, ForLoop)
{
  std::string s{"oooo"};
  std::vector<int> v{1, 1, 1, 1};

  ASSERT_EQ(s.size(), v.size());

  for (auto itr = mf::make_zip_iterator(s.begin(), v.begin()); itr != mf::make_zip_iterator(s.end(), v.end()); ++itr)
  {
    ASSERT_EQ(std::get<0>(*itr), 'o');
    ASSERT_EQ(std::get<1>(*itr), 1);
  }
}


TEST(MultiFieldArray, ForLoopConst)
{
  const std::string s{"oooo"};
  const std::vector<int> v{1, 1, 1, 1};

  ASSERT_EQ(s.size(), v.size());

  for (auto itr = mf::make_zip_iterator(s.begin(), v.begin()); itr != mf::make_zip_iterator(s.cend(), v.end()); ++itr)
  {
    ASSERT_EQ(std::get<0>(*itr), 'o');
    ASSERT_EQ(std::get<1>(*itr), 1);
  }
}
