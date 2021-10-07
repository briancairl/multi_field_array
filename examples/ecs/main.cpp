// C++ Standard Library
#include <iomanip>
#include <iostream>
#include <optional>
#include <vector>

// MF
#include <mf/multi_field_array.hpp>

template <typename... Ts> class Registry
{
public:
  Registry(std::size_t initial_size = 100UL)
  {
    storage_.resize(initial_size);

    // Set available IDs
    available_.resize(initial_size);
    for (std::size_t i = 0; i < initial_size; ++i)
    {
      available_[i] = initial_size - i - 1UL;
    }

    // Set activity flags
    std::fill(storage_.template begin<bool>(), storage_.template end<bool>(), false);
  }

  std::size_t create()
  {
    if (available_.empty())
    {
      const std::size_t previous_size = storage_.size();

      storage_.resize(2UL * previous_size);

      std::fill(storage_.template begin<bool>() + previous_size, storage_.template end<bool>(), false);

      const std::size_t current_size = storage_.size();

      for (std::size_t i = 0; i < previous_size; ++i)
      {
        available_.emplace_back(current_size - i - 1UL);
      }
    }

    // Get next available ID and remove it from the "available" list
    const std::size_t next_id = available_.back();
    available_.pop_back();

    // Set activity flag
    storage_.template get<bool>(next_id) = true;

    return next_id;
  }

  template <typename ComponentT, typename... ComponentCTorArgTs>
  void emplace(const std::size_t id, ComponentCTorArgTs&&... ctor_args)
  {
    storage_.template get<std::optional<ComponentT>>(id).emplace(std::forward<ComponentCTorArgTs>(ctor_args)...);
  }

  inline std::size_t component_count(const std::size_t id) const
  {
    return ((static_cast<bool>(storage_.template get<std::optional<Ts>>(id)) ? 1UL : 0UL) + ...);
  }

  template <typename... ComponentTs> const bool has(const std::size_t id) const
  {
    return (static_cast<bool>(storage_.template get<std::optional<ComponentTs>>(id)) and ...);
  }

  template <typename... ComponentTs> decltype(auto) get(const std::size_t id)
  {
    MF_ASSERT(Registry::has<ComponentTs...>(id));
    if constexpr (sizeof...(ComponentTs) > 1)
    {
      return std::tuple<ComponentTs&...>{storage_.template get<std::optional<ComponentTs>>(id).value()...};
    }
    else
    {
      return std::get<0>(std::tuple<ComponentTs&...>{storage_.template get<std::optional<ComponentTs>>(id).value()...});
    }
  }

  template <typename... ComponentTs> decltype(auto) get(const std::size_t id) const
  {
    MF_ASSERT(Registry::has<ComponentTs...>(id));
    if constexpr (sizeof...(ComponentTs) > 1)
    {
      return std::tuple<const ComponentTs&...>{storage_.template get<std::optional<ComponentTs>>(id).value()...};
    }
    else
    {
      return std::get<0>(
        std::tuple<const ComponentTs&...>{storage_.template get<std::optional<ComponentTs>>(id).value()...});
    }
  }

  template <typename... ComponentTs, typename CallbackT> void for_each(CallbackT&& callback)
  {
    for (std::size_t id = 0; id < storage_.size(); ++id)
    {
      if (Registry::is_active(id) and Registry::has<ComponentTs...>(id))
      {
        std::apply(
          std::forward<CallbackT>(callback),
          std::forward_as_tuple(storage_.template get<std::optional<ComponentTs>>(id).value()...));
      }
    }
  }

  template <typename... ComponentTs, typename CallbackT> void for_each(CallbackT&& callback) const
  {
    for (std::size_t id = 0; id < storage_.size(); ++id)
    {
      if (Registry::is_active(id) and Registry::has<ComponentTs...>(id))
      {
        std::apply(
          std::forward<CallbackT>(callback),
          std::forward_as_tuple(storage_.template get<std::optional<ComponentTs>>(id).value()...));
      }
    }
  }

  void erase(const std::size_t id)
  {
    MF_ASSERT(!storage_.empty());
    MF_ASSERT(storage_.template get<bool>(id));

    // Set to inactive state
    storage_.template get<bool>(id) = false;

    // Deconstruct all components
    mf::tuple_for_each(
      [](auto& component) { component = std::nullopt; }, storage_.template get<std::optional<Ts>...>(id));

    // Make entity available again
    available_.push_back(id);
  }

  void clear()
  {
    for (std::size_t id = 0; id < storage_.size(); ++id)
    {
      if (is_active(id))
      {
        Registry::erase(id);
      }
    }
  }

  inline bool is_active(const std::size_t id) const { return storage_.template get<bool>(id); }

  inline std::size_t size() const { return storage_.size(); }

  inline std::size_t entities() const { return storage_.size() - available_.size(); }

  inline std::size_t available() const { return available_.size(); }

private:
  mf::multi_field_array<bool, std::optional<Ts>...> storage_;
  std::vector<std::size_t> available_;
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
  Registry<Sword, Shield> registry{100};

  // clang-format off
  std::cout << '\n';
  std::cout << std::setw(15) << std::left << "active: " <<
               std::setw(15) << std::left << "id: " <<
               std::setw(15) << std::left << "available: " << '\n';
  // clang-format on

  const std::size_t n_to_create = 2UL * registry.size();

  for (std::size_t i = 0; i < n_to_create; ++i)
  {
    const auto id = registry.create();

    MF_ASSERT(!registry.has<Sword>(id));
    MF_ASSERT(!registry.has<Shield>(id));

    // create a component
    registry.emplace<Sword>(id);
    registry.get<Sword>(id).strength = 2.f;

    // clang-format off
    std::cout << std::setw(15) << std::left << std::boolalpha << registry.is_active(i) <<
                 std::setw(15) << std::left << id <<
                 std::setw(15) << std::left << registry.available() <<
                 std::setw(15) << std::left << "sword.strenth = " << registry.get<Sword>(id).strength << '\n';
    // clang-format on
  }

  // add another component for a few entities
  registry.emplace<Shield>(2);
  registry.emplace<Shield>(5);
  registry.emplace<Shield>(8);

  // Iterate over elements which have a Sword and a Shield
  std::size_t has_both_components = 0;
  registry.for_each<Sword, Shield>(
    [&has_both_components](const auto& sword, const auto& shield) { ++has_both_components; });

  // check sizes before clearing
  std::cout << '\n';
  std::cout << has_both_components << " elements have both Sword and Shield components\n";

  // erase entities
  registry.erase(15);
  registry.erase(34);

  // clang-format off
  std::cout << '\n';
  std::cout << std::setw(15) << std::left << "active: " <<
               std::setw(15) << std::left << "components: " <<
               std::setw(15) << std::left << "id: " << '\n';
  // clang-format on

  for (std::size_t i = 0; i < registry.size(); ++i)
  {
    // clang-format off
    std::cout << std::setw(15) << std::left << std::boolalpha << registry.is_active(i) <<
                 std::setw(15) << std::left << std::boolalpha << registry.component_count(i) <<
                 std::setw(15) << std::left << i << '\n';
    // clang-format on
  }

  // check sizes before clearing
  std::cout << '\n';
  std::cout << "before clearing...\n";
  std::cout << "entities:  " << registry.entities() << '\n';
  std::cout << "available: " << registry.available() << '\n';

  // clear the registry of all enitities
  registry.clear();
  std::cout << '\n';
  std::cout << "after clearing...\n";
  std::cout << "entities:  " << registry.entities() << '\n';
  std::cout << "available: " << registry.available() << '\n';

  std::cout << std::flush;

  return 0;
}