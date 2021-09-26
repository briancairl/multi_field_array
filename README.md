# MF : MultiFieldArray container

This library provides a fully featured multi-field array container. The container is essentially a data-oriented alternative to a `std::vector`.

# Inpsiration

[Entt](https://entt.docsforge.com/master/)

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

```c++

template<typename A, typename B, typename C>
struct MultiFieldArray
{
  std::vector<A> a_data;
  std::vector<B> b_data;
  std::vector<C> c_data;
};

```