[![Pull Request](https://github.com/briancairl/multi_field_array/actions/workflows/pr.yml/badge.svg)](https://github.com/briancairl/multi_field_array/actions/workflows/pr.yml)

# MultiFieldArray container library

C++17 library which implements the well known ["parallel array" or "structure of arrays" (SoA)](https://en.wikipedia.org/wiki/Parallel_array), called a "multi-field array" here. The container is essentially a data-oriented alternative to a `std::vector<S>`, where `S` is a structure with multiple fields, and is, thus, an "array of structures" (AoS).

## Dependencies

### General Use

- C++17 Standard Library

### Unit Tests

- C++17 Standard Library
- [google-test](https://github.com/google/googletest)

### Benchmarking

- C++17 Standard Library
- [google-benchmark](https://github.com/google/benchmark)

## Motivation

Take the following use case:

```c++

#include <iostream>
#include <string>
#include <vector>

struct MyStruct
{
  float a;
  double b;
  std::string c;
};

std::vector<MyStruct> vec;

vec.resize(5);

for (const auto& v : vec)
{
  std::cout << v.a << std::endl;
}

```

When we iterate over `std::vector<MyStruct>`, all fields (a.k.a. comonent, member variable) of each element of type `MyStruct` is loaded into CPU cache, even if we don't need it. Instead, we can take a data-oriented approach where each field is stored in its own buffer. A container which models this approach might look as follows:

```c++

template<typename A, typename B, typename C>
struct MultiFieldArray
{
  // std::vector<X> for exposition only...
  std::vector<A> a_data;
  std::vector<B> b_data;
  std::vector<C> c_data;
};

```

The `MultiFieldArray` follows this approach, and provides accessors for iterating over subsets of the types that it manages. For instance, one can achieve the following:

```c++
mf::multi_field_array<int, float, char> mfa;

for (auto [a, b] : mfa.view<int, float>())
{
  // Iterating over just the int and float fields
}

for (auto [a, b, c] : mfa.view())
{
  // Iterating over all fields
}
```

One can also access a subset of components positionally, like so:

```c++
mf::multi_field_array<int, float, char> mfa;

mfa.resize(10);

auto view = mfa.view<int, float>();

auto [int_ref, float_ref] = view[3];

int_ref = 1;

float_ref = 3.f;

auto [int_ref_b, float_ref_b] = view.at(3);  // with bounds checking
```

Accessing all components positionally is also possible:

```c++
mf::multi_field_array<int, float, char> mfa;

mfa.resize(10);

auto [int_ref, float_ref, char_ref] = mfa[3];

int_ref = 1;

float_ref = 3.f;

char_ref = 'a';

auto [int_ref_b, float_ref_b, char_ref_b] = mfa.at(3);  // with bounds checking
```

# Performance

## Running micro-benchmarking tests
```
bazel test //test/benchmark/... --test_output=all --cache_test_results=no
```

## Example micro-benchmarking output
```
Run on (8 X 4500 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 0.66, 0.48, 0.49

--------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations
--------------------------------------------------------------------------------------
Iteration_One_Of_Two_Fields_MFA                 911860 ns       911152 ns          765   <-- Accessing one field of MultiFieldArray of two field types
Iteration_One_Of_Two_Fields_Vec                1924595 ns      1921561 ns          387   <-- Accessing one field of std::vector<Struct> with two members
Iteration_Two_Of_Two_Fields_MFA                2335226 ns      2331385 ns          301
Iteration_Two_Of_Two_Fields_Vec                2542674 ns      2538851 ns          288
Iteration_Two_Of_Many_Fields_MFA_View          2454603 ns      2450854 ns          265
Iteration_Two_Of_Many_Fields_MFA_All_Fields    2286510 ns      2283481 ns          292
Iteration_Two_Of_Many_Fields_Vec               2908417 ns      2907053 ns          240
Iteration_One_Of_Many_Fields_MFA_View           909963 ns       909970 ns          766
Iteration_One_Of_Many_Fields_MFA_All_Fields     908849 ns       908857 ns          760
Iteration_One_Of_Many_Fields_Vec               2556763 ns      2556768 ns          263
Allocation_Two_Fields_MFA                        20553 ns        20554 ns        34162
Allocation_Two_Fields_Vec                        25640 ns        25641 ns        27229
Allocation_Many_Fields_MFA                       20441 ns        20441 ns        33742
Allocation_Many_Fields_Vec                       35104 ns        35104 ns        19913
================================================================================
```

# Inspiration

The `MultiFieldArray` is sort of like a very (very) simple Entity-component system (ECS). Unlike a pratical ECS, all component types need to be known "ahead of time" when defining a concrete multi-field array type. Additionally, memory will be allocated to hold all components for all enitities, whether you need it or not.

In this sense, it implements a "special case" ECS with the absolute minimal access indirection. Since there are many cases where allocating for all components at once is actually necessary, it made sense to implement even though there are other more general purposes libraries out there.

For a truer, "swiss army knife," ECS check out the outstanding [Entt](https://github.com/skypjack/entt) library.


# TODOs

- [ ] Add `shrink_to_fit` method to `BasicMultiFieldArray`
- [x] Add "single allocation" strategy option, which will allocate all field memory at once, rather than with separate allocation calls
- [ ] Add `CMake` build configuration
- [ ] Add `CMake` CI step
- [ ] Add Doxygen documentation page releases
- [x] Add benchmark tests
