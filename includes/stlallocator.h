#ifndef STLALLOCATOR_H
#define STLALLOCATOR_H

#include <cstddef>
#include <limits>
#include <utility>

namespace y {
template <typename Tp, template<typename T >typename _Alloc> class StlAllocator {
private:
  _Alloc<Tp> *m_alloc;
  friend _Alloc<Tp>;

public:
  using value_type = Tp;
  using pointer = Tp*;
  using const_pointer = const Tp*;
  using reference = Tp&;
  using const_reference = const Tp&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  StlAllocator() noexcept : m_alloc(static_cast<_Alloc<Tp> *>(this)) {}

  StlAllocator(const StlAllocator &) noexcept
      : m_alloc(static_cast<_Alloc<Tp> *>(this)) {}

  StlAllocator(StlAllocator &&) noexcept
      : m_alloc(static_cast<_Alloc<Tp> *>(this)) {}

  template <typename U>
  StlAllocator(const StlAllocator<U, _Alloc> &) noexcept {}

  ~StlAllocator() noexcept {}

  Tp *allocate(std::size_t num) {
    return static_cast<Tp *>(m_alloc->do_alloc(num * sizeof(Tp)));
  }

  void deallocate(Tp *ptr, std::size_t) { m_alloc->do_free(ptr); }

  template <typename Up, typename... Args>
  void construct(Up *ptr, Args &&...args) {
    ::new (ptr) Up(std::forward<Args>(args)...);
  }

  template<typename Up>
  void destroy(Tp *ptr) { ptr->~Up(); }

  size_type max_size() const noexcept {
    return std::numeric_limits<size_type>::max() / sizeof(size_type);
  }

  template <typename U>
  bool operator==(const StlAllocator<U, _Alloc> &) const noexcept {
    return true;
  }

  template <typename U>
  bool operator!=(const StlAllocator<U, _Alloc> &) const noexcept {
    return false;
  }

  template<typename U>
  struct rebind {
    using other = _Alloc<U>;
  };
};
} // namespace y

#define ALLOCATOR_PATCH(STL_IMPL_TYPE)                                         \
public:                                                                        \
  template <typename U> STL_IMPL_TYPE(const STL_IMPL_TYPE<U> &) noexcept {}    \
  STL_IMPL_TYPE() = default;

#endif// STLALLOCATOR_H