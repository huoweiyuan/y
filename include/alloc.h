#ifndef ALLOC_H
#define ALLOC_H

namespace y 
{

class Allocator
{
 protected:
  ushort EXTRA_MEM_SIZE = 0;

 private:
  // for new/delete
   static constexpr const short OBJ_NUM_HEAD_SIZE = 8;
   static constexpr const short OBJ_PTR_HEAD_SIZE = 8;
   static constexpr const short READ_OBJ_NUM_OFFSET = -(OBJ_NUM_HEAD_SIZE) -(OBJ_PTR_HEAD_SIZE);
   static constexpr const short READ_OBJ_PTR_OFFSET = -(OBJ_PTR_HEAD_SIZE);

 private:
   virtual char* alloc_memory(std::size_t size)
   {
     return static_cast<char*>(::malloc(size));
   }
   virtual char* realloc_memory(void *ptr, size_t size)
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
   Allocator() noexcept = default;
   virtual ~Allocator() noexcept = default;

 public:
   static Allocator* get_allocator_ptr(void *ptr) {
     if (ptr == nullptr)
       return nullptr;
     return *(reinterpret_cast<Allocator**>(
         static_cast<char*>(ptr) + READ_OBJ_PTR_OFFSET));
   }
};

inline void* operator new (size_t size, Allocator *alloc) noexcept
{
  return alloc->alloc(size);
}

inline void* operator new[] (size_t size, Allocator *alloc) noexcept
{
  return alloc->alloc(size);
}

inline void operator delete(void *pt) noexcept {
  Allocator *allocator = nullptr; 
  if (ptr == nullptr ||
    (allocator = Allocator::get_allocator_ptr(ptr)) == nullptr)
    return;
  allocator->free(ptr);
}

inline void operator delete[](void *ptr) noexcept
{
  Allocator *allocator = nullptr; 
  if (ptr == nullptr ||
    (allocator = Allocator::get_allocator_ptr(ptr)) == nullptr)
    return;
  allocator->free(ptr);
}

}

#endif// ALLOC_H