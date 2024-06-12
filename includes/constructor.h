#ifndef Y_CONSTRUCT_H
#define Y_CONSTRUCT_H

#include "allocator.h"
#include <cstddef>
#include <limits>
#include <new>
#include <utility>

namespace y {
// template <typename _Tp, typename _Alloc> class ConstructorT : public Allocator<_Alloc> {
// public:
//   using value_type = _Tp;
// 
// public:
//   ConstructorT() noexcept = default;
//   template <typename U> ConstructorT(const ConstructorT<U> &) noexcept {}
//   ~ConstructorT() = default;
// 
// public:
//   _Tp* allocate(size_t num, const void* hint = 0) {
//     if (num > std::numeric_limits<size_t>::max() / sizeof(_Tp)) {
//       throw std::bad_alloc();
//     }
//     void *ptr = Allocator<_Tp>::alloc(num * sizeof(_Tp));
//     if (!ptr) {
//       throw std::bad_alloc();
//     }
//     return ptr;
//   }
// 
//   void deallocate(_Tp* ptr, size_t num) noexcept {
//     free(ptr);
//   }
// 
//   template <typename U, typename... Args> void construct(U *p, Args &&...args) {
//     ::new ((void *)p) U(std::forward<Args>(args)...);
//   }
//   
//   template <typename U> void destroy(U *p) { p->~U(); }
// 
//   template <typename U> struct rebind {
//     using other = ConstructorT<U>;
//   };
// 
//   bool operator==(const ConstructorT &other) const noexcept {
//     return true; // 所有标准分配器都相等
//   }
// 
//   bool operator!=(const ConstructorT &other) const noexcept {
//     return false; // 所有标准分配器都相等
//   }
// };

template <typename _Alloc> class Constructor : public Allocator<_Alloc> {
private:
  /* |NUM|xxx| */
  static const size_t OBJ_NUM_SIZE = sizeof(size_t);
  static const size_t HEADER_TOTAL_SIZE = OBJ_NUM_SIZE;

  static const size_t WRITE_OBJ_NUM_OFFSET = 0;
  static const size_t READ_OBJ_NUM_OFFSET = -OBJ_NUM_SIZE;

private:
  Constructor() = default;
  friend _Alloc;

public:
  template <typename ClassName, size_t Num, typename... P>
  ClassName *create_obj(P &&...args) {
    const size_t _class_size = sizeof(ClassName);
    void *_ptr = Allocator<_Alloc>::alloc(_class_size * Num + HEADER_TOTAL_SIZE);
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
    char *_ptr = static_cast<char *>((void *)_destruct_ptr);

    void *_num_ptr = _ptr + READ_OBJ_NUM_OFFSET;
    size_t _obj_num = *static_cast<size_t *>(_num_ptr);
    for (size_t i = 0; i < _obj_num; i++) {
      (_destruct_ptr + i)->~ClassName();
    }

    void *_alloced_ptr = _ptr - HEADER_TOTAL_SIZE;
    Allocator<_Alloc>::free(_alloced_ptr);
  }
};

template <typename ClassName, size_t Num = 1, typename _Constructor,
          typename... P>
ClassName *g_new(_Constructor *constructor, P &&...args) {
  if (constructor == nullptr)
    return nullptr;
  ClassName *_ptr = constructor->template create_obj<ClassName, Num>(
      std::forward<P>(args)...);
  return _ptr;
}

template <typename ClassName, typename _Constructor>
void g_delete(_Constructor *constructor, ClassName *ptr) {
  if (constructor == nullptr)
    return;
  constructor->template delete_obj(ptr);
}

} // namespace y
#endif // Y_CONSTRUCT_H