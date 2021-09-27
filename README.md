# MF : MultiFieldArray container library

This library provides a fully featured multi-field array container. The container is essentially a data-oriented alternative to a `std::vector`.

## Motivation

Imagine the following typical case:

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

When we iterate over `std::vector<MyStruct>`, all field/component of each element is loaded into CPU cache, even if we don't need it. Instead, we can take a data-oriented approach where each field is stored in its own buffer.

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

The `MultiFieldArray` follows this approach, and provides accessors for iterating over subsets of the types that it manages. For instance, one can achieve the following with this library

```c++
MultiFieldArray<Fields<int, float, char>> array;

for (auto [a, b] : array.view<int, float>())
{
  // Iterating over just the int and float fields
}
```

# Inspiration

The `MultiFieldArray` is sort of like an entity component system, but requires that fields exist for each enitity (at each index). In this sense, its much simpler than an ECS. Moreover, its sort of a special case of an ECS where we want all entities to have all specified components.

For a truer, "swiss army knife," ECS check out the outstanding [Entt](https://github.com/skypjack/entt) library.
