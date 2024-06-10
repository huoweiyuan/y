#ifndef Y_ALLOC_H
#define Y_ALLOC_H

#include <cstddef>
#include <utility>

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

class Constructor : protected Allocator {
private:
  /* |NUM|xxx| */
  static const size_t OBJ_NUM_SIZE = sizeof(size_t);
  static const size_t HEADER_TOTAL_SIZE = OBJ_NUM_SIZE;
  
  static const size_t WRITE_OBJ_NUM_OFFSET = 0;
  static const size_t READ_OBJ_NUM_OFFSET = -OBJ_NUM_SIZE;

public:
  template <typename ClassName, size_t Num, typename... P>
  ClassName *create_obj(P &&...args) {
    const size_t _class_size = sizeof(ClassName);
    void *_ptr = alloc(_class_size * Num + HEADER_TOTAL_SIZE);
    if (_ptr == nullptr)
      return nullptr;

    *static_cast<size_t *>(_ptr) = Num;

    void *_obj_ptr = static_cast<char *>(_ptr) + HEADER_TOTAL_SIZE;
    ClassName *_construct_ptr = static_cast<ClassName *>(_obj_ptr);
    for (size_t i = 0; i < Num; i++) {
      new (_construct_ptr + i) ClassName(std::forward<P>(args)...);
    }
    return _construct_ptr;
  }

  template <typename ClassName> void delete_obj(ClassName *_destruct_ptr) {
    if (_destruct_ptr == nullptr)
      return;
    char *_ptr = static_cast<char *>((void*)_destruct_ptr);

    void *_num_ptr = _ptr + READ_OBJ_NUM_OFFSET;
    size_t _obj_num = *static_cast<size_t *>(_num_ptr);
    for (size_t i = 0; i < _obj_num; i++) {
      (_destruct_ptr + i)->~ClassName();
    }

    void *_alloced_ptr = _ptr - HEADER_TOTAL_SIZE ;
    free(_alloced_ptr);
  }

public:
  static void *get_allocator_ptr(void *ptr);
};

class SimpleSysAlloc : public Constructor {
private:
  void *alloc_memory(size_t size) override;
  void *realloc_memory(void *ptr, size_t size) override;
  void free_memory(void *ptr) override;
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

template <typename ClassName, size_t Num = 1, typename... P>
ClassName *g_new(Constructor *constructor, P &&...args) {
  if (constructor == nullptr)
    return nullptr;
  ClassName *_ptr =
      constructor->create_obj<ClassName, Num>(std::forward<P>(args)...);
  return _ptr;
}

template <typename ClassName> void g_delete(ClassName *ptr) {
  Constructor *constructor = nullptr;
  if (ptr == nullptr || (constructor = static_cast<Constructor *>(
                             Constructor::get_allocator_ptr(ptr))) == nullptr)
    return;
  constructor->delete_obj(ptr);
}
} // namespace y

#endif // Y_ALLOC_H
