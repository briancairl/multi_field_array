// C++ Standard Library
#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

// MF
#include <mf/multi_field_array.hpp>

template <typename... Ts> class ECSRegistry
{
public:
  ECSRegistry(std::size_t initial_size = 100UL) : in_use_{0UL}
  {
    storage_.resize(initial_size);
    available_.resize(initial_size);
    for (std::size_t i = 0; i < initial_size; ++i)
    {
      available_[i] = initial_size - i - 1UL;
    }
  }

  std::size_t create()
  {
    if (available_.empty())
    {
      const std::size_t previous_size = storage_.size();

      storage_.resize(2UL * previous_size);

      const std::size_t current_size = storage_.size();

      for (std::size_t i = 0; i < previous_size; ++i)
      {
        available_.emplace_back(current_size - i - 1UL);
      }
    }

    ++in_use_;

    const std::size_t next_id = available_.back();
    available_.pop_back();
    return next_id;
  }

  template <typename ComponentT, typename... ComponentCTorArgTs>
  void emplace(const std::size_t id, ComponentCTorArgTs&&... ctor_args)
  {
    storage_.template get<std::optional<ComponentT>>(id).emplace(std::forward<ComponentCTorArgTs>(ctor_args)...);
  }

  template <typename ComponentT> bool has(const std::size_t id)
  {
    return static_cast<bool>(storage_.template get<std::optional<ComponentT>>(id));
  }

  template <typename ComponentT> const bool has(const std::size_t id) const
  {
    return static_cast<bool>(storage_.template get<std::optional<ComponentT>>(id));
  }

  template <typename ComponentT> ComponentT& get(const std::size_t id)
  {
    return storage_.template get<std::optional<ComponentT>>(id).value();
  }

  template <typename ComponentT> const ComponentT& get(const std::size_t id) const
  {
    return storage_.template get<std::optional<ComponentT>>(id).value();
  }

  void erase(const std::size_t id)
  {
    MF_ASSERT(!storage_.empty());

    // TODO(implement)
  }

  inline std::size_t size() const { return storage_.size(); }

  inline std::size_t in_use() const { return in_use_; }

  inline std::size_t available() const { return available_.size(); }

private:
  mf::multi_field_array<std::optional<Ts>...> storage_;
  std::vector<std::size_t> available_;
  std::size_t in_use_;
};

struct Sword
{
  float strength;
};

struct Shield
{
  char type;
};

int main(int argc, char** argv)
{
  ECSRegistry<Sword, Shield> registry;

  std::cout << std::setw(15) << std::left << "iteration: " << std::setw(15) << std::left << "id: " << std::setw(15)
            << std::left << "available: " << '\n';

  const std::size_t n_to_create = 2UL * registry.size();

  for (std::size_t i = 0; i < n_to_create; ++i)
  {
    const auto id = registry.create();

    if (registry.has<Sword>(id) or registry.has<Shield>(id))
    {
      throw std::runtime_error{"these shouldn't be setup yet"};
    }

    registry.emplace<Sword>(id, Sword{.strength = 1.f});

    std::cout << std::setw(15) << std::left << i << std::setw(15) << std::left << id << std::setw(15) << std::left
              << registry.available() << std::setw(15) << std::left
              << "sword.strenth = " << registry.get<Sword>(id).strength << '\n';
  }

  std::cout << std::flush;

  return 0;
}