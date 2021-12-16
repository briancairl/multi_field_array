/**
 * @copyright 2021 MF
 * @author Brian Cairl
 */
#pragma once

// C++ Standard Library
#include <iterator>
#include <type_traits>

namespace mf
{

/**
 * @brief Adaptor for reversing iterator direction
 */
template <typename UnderlyingIteratorT> class reverse_iterator_adapter
{
public:
  constexpr bool operator==(const reverse_iterator_adapter& other) const { return u_ == other.u_; }
  constexpr bool operator!=(const reverse_iterator_adapter& other) const { return u_ == other.u_; }
  constexpr bool operator<(const reverse_iterator_adapter& other) const { return u_ > other.u_; }
  constexpr bool operator>(const reverse_iterator_adapter& other) const { return u_ < other.u_; }
  constexpr bool operator<=(const reverse_iterator_adapter& other) const
  {
    return this->operator==(other) or this->operator<(other);
  }
  constexpr bool operator>=(const reverse_iterator_adapter& other) const
  {
    return this->operator==(other) or this->operator>(other);
  }

  constexpr reverse_iterator_adapter& operator=(const reverse_iterator_adapter& other)
  {
    this->u_ = other.u_;
    return *this;
  }

  constexpr std::ptrdiff_t operator-(const reverse_iterator_adapter& other) const { return other.u_ - this->u_; }

  constexpr reverse_iterator_adapter operator-(const std::ptrdiff_t offset) const
  {
    reverse_iterator_adapter result{*this};
    result.u_ += offset;
    return result;
  }

  constexpr reverse_iterator_adapter operator+(const std::ptrdiff_t offset) const
  {
    reverse_iterator_adapter result{*this};
    result.u_ -= offset;
    return result;
  }

  constexpr reverse_iterator_adapter& operator-=(const std::ptrdiff_t offset)
  {
    this->u_ += offset;
    return *this;
  }

  constexpr reverse_iterator_adapter& operator+=(const std::ptrdiff_t offset)
  {
    this->u_ -= offset;
    return *this;
  }

  constexpr reverse_iterator_adapter& operator--()
  {
    ++this->u_;
    return *this;
  }

  constexpr reverse_iterator_adapter operator--(int)
  {
    reverse_iterator_adapter prev{*this};
    ++prev.u_;
    return prev;
  }

  constexpr reverse_iterator_adapter& operator++()
  {
    --this->u_;
    return *this;
  }

  constexpr reverse_iterator_adapter operator++(int)
  {
    reverse_iterator_adapter prev{*this};
    --prev.u_;
    return prev;
  }

  constexpr decltype(auto) operator*() { return *(this->u_); }

  constexpr decltype(auto) operator*() const { return *(this->u_); }

  constexpr decltype(auto) operator-> ()
  {
    if constexpr (std::is_pointer<UnderlyingIteratorT>())
    {
      return this->u_;
    }
    else
    {
      return this->u_.operator->();
    }
  }

  constexpr decltype(auto) operator-> () const
  {
    if constexpr (std::is_pointer<UnderlyingIteratorT>())
    {
      return this->u_;
    }
    else
    {
      return this->u_.operator->();
    }
  }

  explicit reverse_iterator_adapter(const UnderlyingIteratorT& underlying) : u_{underlying} {}

  reverse_iterator_adapter(const reverse_iterator_adapter& underlying) = default;

  reverse_iterator_adapter(reverse_iterator_adapter&& underlying) = default;

private:
  UnderlyingIteratorT u_;
};

}  // namespace mf

namespace std
{

/**
 * @brief Specialization of \c std::iterator_traits for a valid \c reverse_iterator_adapter template instance
 */
template <typename UnderlyingIteratorT>
struct iterator_traits<::mf::reverse_iterator_adapter<UnderlyingIteratorT>> : std::iterator_traits<UnderlyingIteratorT>
{};

}  // namespace std
