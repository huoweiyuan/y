#ifndef Y_ALLOC_H
#define Y_ALLOC_H

#include <cstddef>

namespace y {

template<typename T>
class Allocator {
private:
  /* |allocator ptr|xxxx| */
  static const size_t ALLOCATOR_PTR_SIZE = sizeof(void *);
  static const size_t HEADER_TOTAL_SIZE = ALLOCATOR_PTR_SIZE;

  /* OFFSET */
  static const size_t WRITE_ALLOCATOR_PTR_OFFSET = 0;
  static const size_t READ_ALLOCATOR_PTR_OFFSET = -(ALLOCATOR_PTR_SIZE);

private:
  T* m_derived_ptr;

public:
  Allocator() : m_derived_ptr(static_cast<T *>(this)) {}

public:
  void *alloc(size_t size) {
    void *_ptr = m_derived_ptr->alloc_memory(size + HEADER_TOTAL_SIZE);
    *(static_cast<void **>(_ptr)) = this;
    return static_cast<char *>(_ptr) + HEADER_TOTAL_SIZE;
  }

  void *realloc(void *ptr, size_t size) {
    void *_realloc_ptr = m_derived_ptr->realloc_memory(
        static_cast<char *>(ptr) - HEADER_TOTAL_SIZE, size + HEADER_TOTAL_SIZE);
    return static_cast<char *>(_realloc_ptr) + HEADER_TOTAL_SIZE;
  }

  void free(void *ptr) {
    m_derived_ptr->free_memory(static_cast<char *>(ptr) - HEADER_TOTAL_SIZE);
  }
};

} // namespace y

namespace y {
template <typename _Alloc>
inline void *g_alloc(_Alloc *allocator, size_t size) {
  if (allocator == nullptr)
    return nullptr;
  return allocator->alloc(size);
}

template <typename _Alloc>
inline void *g_realloc(_Alloc *allocator, void *ptr, size_t size) {
  if (allocator == nullptr)
    return nullptr;
  return allocator->realloc(ptr, size);
}

template <typename _Alloc> inline void g_free(_Alloc *allocator, void *ptr) {
  if (allocator == nullptr)
    return;
  allocator->free(ptr);
}

} // namespace y

#endif // Y_ALLOC_H
