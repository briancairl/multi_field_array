/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */

// C++ Standard Library
#include <memory>
#include <string>

// GTest
#include <gtest/gtest.h>

// MF
#include <mf/multi_allocator_adapter.hpp>

TEST(MultiAllocatorAdapter, PerFieldAllocations)
{
  mf::multi_allocator_adapter<float, int, std::string> allocator;

  auto tuple_of_ptrs = allocator.allocate(10);

  allocator.deallocate(tuple_of_ptrs, 10);
}


TEST(MultiAllocatorAdapter, SinglePassAllocation)
{
  mf::single_allocator_adapter<float, int, std::string> allocator;

  auto tuple_of_ptrs = allocator.allocate(10);

  allocator.deallocate(tuple_of_ptrs, 10);
}
