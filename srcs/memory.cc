#include "debug.h"
#include <cstddef>
#include <cstdlib>

#include "allocator.h"
#include "constructor.h"

void *y::Allocator::alloc(size_t size) {
  void *_ptr = alloc_memory(size + HEADER_TOTAL_SIZE);
  *(static_cast<void**>(_ptr)) = this;
  return static_cast<char*>(_ptr) + HEADER_TOTAL_SIZE;
}

void *y::Allocator::realloc(void *ptr, size_t size) {
  void *_realloc_ptr = realloc_memory(
      static_cast<char *>(ptr) - HEADER_TOTAL_SIZE, size + HEADER_TOTAL_SIZE);
  return static_cast<char*>(_realloc_ptr) + HEADER_TOTAL_SIZE;
}

void y::Allocator::free(void *ptr) {
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

