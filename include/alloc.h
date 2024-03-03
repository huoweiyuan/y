#ifndef ALLOC_H
#define ALLOC_H

#include <atomic>
#include <mutex>

namespace y 
{
  class Allocator;
  extern Allocator *g_allocator;
  class Allocator
  {
  protected:
    ushort EXTRA_MEM_SIZE = 0;

  private:
    // for new/delete
    static constexpr const short OBJ_NUM_HEAD_SIZE = 8;
    static constexpr const short OBJ_PTR_HEAD_SIZE = 8;
    static constexpr const short READ_OBJ_NUM_OFFSET = -(OBJ_NUM_HEAD_SIZE) - (OBJ_PTR_HEAD_SIZE);
    static constexpr const short READ_OBJ_PTR_OFFSET = -(OBJ_PTR_HEAD_SIZE);

  private:
    static std::mutex s_mutex;
    std::atomic_bool m_as_default_allocator;
    friend void g_allocator_init(Allocator *);

  private:
    virtual char *
    alloc_memory(std::size_t size)
    {
      return static_cast<char *>(::malloc(size));
    }
    virtual char *realloc_memory(void *ptr, size_t size)
    {
      return static_cast<char*>(::realloc(ptr, size));
   }
   virtual void free_memory(void *ptr)
   {
     ::free(ptr);
   }

 public:
   // |obj num|obj pointer|xxxx
   template<typename ClassName, size_t Num, typename... P> 
     ClassName* create_obj(P&&... args)
     {
       const size_t _class_size = sizeof(ClassName);
       char *_ptr = 
         alloc_memory(_class_size * Num + OBJ_NUM_HEAD_SIZE + OBJ_PTR_HEAD_SIZE);
       if (_ptr == nullptr)
         return nullptr;

       *(reinterpret_cast<size_t*>(_ptr)) = Num;
       _ptr += OBJ_NUM_HEAD_SIZE;
       *(reinterpret_cast<Allocator**>(_ptr)) = this;
       _ptr += OBJ_PTR_HEAD_SIZE;

       ClassName *_construct_ptr = reinterpret_cast<ClassName*>(_ptr);
       for (size_t i = 0; i < Num; i++)
       {
         new(_construct_ptr + i) ClassName(std::forward<P>(args)...);
       }
       return _construct_ptr;
     }

   template<typename ClassName>
     void delete_obj(ClassName *_destruct_ptr)
     {
       if (_destruct_ptr == nullptr)
         return;
       char *_alloced_ptr =
         reinterpret_cast<char*>(_destruct_ptr) + READ_OBJ_NUM_OFFSET;
       const size_t _obj_num = *(reinterpret_cast<size_t*>(_alloced_ptr));
       for (size_t i = 0; i < _obj_num; i++)
       {
         (_destruct_ptr + i)->~ClassName();
       }
       free_memory(_alloced_ptr);
     }

 public:
   // |obj pointer|xxxx
   void* alloc(size_t size) {
     char* _ptr = alloc_memory(size + OBJ_PTR_HEAD_SIZE);
     if (_ptr == nullptr) 
       return nullptr;
     *(reinterpret_cast<Allocator**>(_ptr)) = this;
     return _ptr + OBJ_PTR_HEAD_SIZE;
   }

   void* realloc(void *ptr, size_t size) {
     if (ptr == nullptr) 
       return nullptr;
     char *_ptr = realloc_memory(static_cast<char*>(ptr) + READ_OBJ_PTR_OFFSET,
                                 size + OBJ_PTR_HEAD_SIZE);
     if (_ptr == nullptr)
       return nullptr;
     return _ptr + OBJ_PTR_HEAD_SIZE;
   }

   void free(void *ptr) {
     if (ptr == nullptr)
       return;
     free_memory(static_cast<char*>(ptr) + READ_OBJ_PTR_OFFSET);
   }

 public:
   Allocator() noexcept
   : m_as_default_allocator(false)
   {}
   Allocator(bool as_default_allocator) noexcept
   : m_as_default_allocator(false)
   {
     if (as_default_allocator)
       set_as_default_allocator();
   }
   virtual ~Allocator() noexcept;

 public:
   static Allocator* get_allocator_ptr(void *ptr) {
     if (ptr == nullptr)
       return nullptr;
     return *(reinterpret_cast<Allocator**>(
         static_cast<char*>(ptr) + READ_OBJ_PTR_OFFSET));
   }

   void set_as_default_allocator()
   {
     std::lock_guard<std::mutex> lk(s_mutex);
     if (g_allocator)
       g_allocator->m_as_default_allocator = false;
     m_as_default_allocator = true;
     g_allocator = this;
   }

};

std::mutex Allocator::s_mutex;

Allocator  g_default_allocator(true);
y::Allocator *g_allocator = &g_default_allocator;

Allocator::~Allocator()
{
  if (m_as_default_allocator) {
    std::lock_guard<std::mutex> lk(s_mutex);
    m_as_default_allocator = false;
    g_allocator = &g_default_allocator;
    g_default_allocator.m_as_default_allocator = true;
  }
}

}

void* operator new (size_t size, y::Allocator *alloc) noexcept
{
  return alloc->alloc(size);
}

void* operator new[] (size_t size, y::Allocator *alloc) noexcept
{
  return alloc->alloc(size);
}

void* operator new(std::size_t size) noexcept
{
  return y::g_allocator->alloc(size);
}
 
void* operator new[](std::size_t size)
{
  return y::g_allocator->alloc(size);
}
 
void operator delete(void* ptr) noexcept
{
  y::Allocator *allocator = nullptr; 
  if (ptr == nullptr ||
    (allocator = y::Allocator::get_allocator_ptr(ptr)) == nullptr)
    return;
  allocator->free(ptr);
}
 
void operator delete(void* ptr, std::size_t size) noexcept
{
  y::Allocator *allocator = nullptr;
  if (ptr == nullptr ||
      (allocator = y::Allocator::get_allocator_ptr(ptr)) == nullptr)
    return;
  allocator->free(ptr);
}

void operator delete[](void* ptr) noexcept
{
  y::Allocator *allocator = nullptr; 
  if (ptr == nullptr ||
    (allocator = y::Allocator::get_allocator_ptr(ptr)) == nullptr)
    return;
  allocator->free(ptr);
}
 
void operator delete[](void* ptr, std::size_t size) noexcept
{
  y::Allocator *allocator = nullptr;
  if (ptr == nullptr ||
      (allocator = y::Allocator::get_allocator_ptr(ptr)) == nullptr)
    return;
  allocator->free(ptr);
}

#endif// ALLOC_H