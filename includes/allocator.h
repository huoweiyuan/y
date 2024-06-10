#ifndef Y_ALLOC_H
#define Y_ALLOC_H

#include <cstddef>

namespace y {

class Allocator {
private:
  /* |allocator ptr|xxxx| */
  static const size_t ALLOCATOR_PTR_SIZE = sizeof(void *);
  static const size_t HEADER_TOTAL_SIZE = ALLOCATOR_PTR_SIZE;

  /* OFFSET */
  static const size_t WRITE_ALLOCATOR_PTR_OFFSET = 0;
  static const size_t READ_ALLOCATOR_PTR_OFFSET = -(ALLOCATOR_PTR_SIZE);

private:
  virtual void *alloc_memory(size_t size) = 0;
  virtual void *realloc_memory(void *ptr, size_t size) = 0;
  virtual void free_memory(void *ptr) = 0;

protected:
  void *alloc(size_t size);
  void *realloc(void *ptr, size_t size);
  void free(void *ptr);

  friend inline void *g_alloc(Allocator *, size_t);
  friend inline void *g_realloc(void *, size_t);
  friend inline void g_free(void *);

  static void *get_allocator_ptr(void *ptr);
};

} // namespace y

namespace y {
inline void *g_alloc(Allocator *allocator, size_t size) {
  if (allocator == nullptr)
    return nullptr;
  return allocator->alloc(size);
}

inline void *g_realloc(void *ptr, size_t size) {
  Allocator *allocator = nullptr;
  return allocator->realloc(ptr, size);
}

inline void g_free(void *ptr) {
  Allocator *allocator = nullptr;
  if (ptr == nullptr || (allocator = static_cast<Allocator *>(
                             Allocator::get_allocator_ptr(ptr))) == nullptr)
    return;
  allocator->free(ptr);
}

} // namespace y

#endif // Y_ALLOC_H
