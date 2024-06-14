#ifndef Y_CONSTRUCT_H
#define Y_CONSTRUCT_H

#include <cstddef>
#include <utility>

namespace y {
template <typename _Alloc> class Constructor {
private:
  struct Head_struct {
    size_t num;
  };
  _Alloc *m_alloc;

private:
  Constructor() : m_alloc(static_cast<_Alloc *>(this)) {}
  friend _Alloc;

public:
  template <typename ClassName, size_t Num, typename... P>
  ClassName *create_obj(P &&...args) {
    Head_struct *_head = (Head_struct *)m_alloc->do_alloc(
        Num * sizeof(ClassName) + sizeof(Head_struct));
    if (_head == nullptr)
      return nullptr;

    _head->num = Num;
    ClassName *_construct = (ClassName *)((char *)_head + sizeof(Head_struct));
    for (size_t i = 0; i < Num; i++) {
      new (_construct + i) ClassName(std::forward<P>(args)...);
    }
    return _construct;
  }

  template <typename ClassName> void delete_obj(ClassName *_destruct) {
    if (_destruct == nullptr)
      return;
    Head_struct *_head =
        (Head_struct *)((char *)_destruct - sizeof(Head_struct));

    for (size_t i = 0; i < _head->num; i++) {
      (_destruct + i)->~ClassName();
    }
    m_alloc->do_free(_head);
  }
};

template <typename ClassName, size_t Num = 1, typename _Constructor,
          typename... P>
ClassName *g_new(_Constructor *constructor, P &&...args) {
  ClassName *_ptr = constructor->template create_obj<ClassName, Num>(
      std::forward<P>(args)...);
  return _ptr;
}

template <typename ClassName, typename _Constructor>
void g_delete(_Constructor *constructor, ClassName *ptr) {
  constructor->template delete_obj(ptr);
}

} // namespace y
#endif // Y_CONSTRUCT_H