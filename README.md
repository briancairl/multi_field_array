[![Pull Request](https://github.com/briancairl/mf/actions/workflows/pr.yml/badge.svg)](https://github.com/briancairl/mf/actions/workflows/pr.yml)

# MF : MultiFieldArray container library

C++17 library which provides a fully featured multi-field array container. The container is essentially a data-oriented alternative to a `std::vector`.

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

# Inspiration

The `MultiFieldArray` is sort of like an entity component system, but requires that fields exist for each enitity (at each index). In this sense, its much simpler than an ECS. Moreover, its sort of a special case of an ECS where we want all entities to have all specified components.

For a truer, "swiss army knife," ECS check out the outstanding [Entt](https://github.com/skypjack/entt) library.


# TODOs

- [ ] Add `shrink_to_fit` method to `BasicMultiFieldArray`
- [ ] Add "single allocation" strategy option, which will allocate all field memory at once, rather than with separate allocation calls
- [ ] Add `CMake` build configuration
- [ ] Add `CMake` CI step
- [ ] Add Doxygen documentation page releases
- [ ] Adds benchmark tests
