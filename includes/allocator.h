#ifndef Y_ALLOC_H
#define Y_ALLOC_H

#include <cstddef>

namespace y {

template <typename _Alloc> class Allocator {
private:
  _Alloc* m_alloc;
  friend _Alloc;
private:
  Allocator() : m_alloc(static_cast<_Alloc *>(this)) {}
  friend _Alloc;

public:
  void *alloc(size_t size) { return m_alloc->do_alloc(size); }

  void *realloc(void *p, size_t size) { return m_alloc->do_realloc(p, size); }

  void free(void *p) { m_alloc->do_free(p); }
};

} // namespace y

namespace y {
template <typename _Alloc>
inline void *g_alloc(_Alloc *allocator, size_t size) {
  return allocator->alloc(size);
}

template <typename _Alloc>
inline void *g_realloc(_Alloc *allocator, void *ptr, size_t size) {
  return allocator->realloc(ptr, size);
}

template <typename _Alloc> inline void g_free(_Alloc *allocator, void *ptr) {
  allocator->free(ptr);
}

} // namespace y

#endif // Y_ALLOC_H
