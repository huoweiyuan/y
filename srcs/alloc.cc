#include "alloc.h"
#include "debug.h"
#include <cstddef>
#include <cstdlib>

void *y::Allocator::alloc(size_t size) {
  void *_ptr = alloc_memory(size + HEADER_TOTAL_SIZE);
  PRINT_DEBUG("alloc_memory:\t%p\n", _ptr);
  *(static_cast<void**>(_ptr)) = this;
  return static_cast<char*>(_ptr) + HEADER_TOTAL_SIZE;
}

void *y::Allocator::realloc(void *ptr, size_t size) {
  void *_realloc_ptr = realloc_memory(
      static_cast<char *>(ptr) - HEADER_TOTAL_SIZE, size + HEADER_TOTAL_SIZE);
  return static_cast<char*>(_realloc_ptr) + HEADER_TOTAL_SIZE;
}

void y::Allocator::free(void *ptr) {
  PRINT_DEBUG("free_memory:\t%p\n", static_cast<char *>(ptr) - HEADER_TOTAL_SIZE);
  free_memory(static_cast<char *>(ptr) - HEADER_TOTAL_SIZE);
}

void *y::Allocator::get_allocator_ptr(void *ptr) {
  void *_ptr = static_cast<char*>(ptr) + READ_ALLOCATOR_PTR_OFFSET;
  return *static_cast<void**>(_ptr);
}

void *y::Constructor::get_allocator_ptr(void *ptr) {
  return Allocator::get_allocator_ptr(static_cast<char *>(ptr) -
                                      HEADER_TOTAL_SIZE);
}

void *y::SimpleSysAlloc::alloc_memory(size_t size) {
  return ::malloc(size);
}

void *y::SimpleSysAlloc::realloc_memory(void *ptr, size_t size) {
  return ::realloc(ptr, size);
}

void y::SimpleSysAlloc::free_memory(void *ptr) {
  return ::free(ptr);
}